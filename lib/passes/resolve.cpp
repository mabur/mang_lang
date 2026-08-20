#include "resolve.h"

#include <carma/carma.h>
#include <carma/carma_table.h>

#include "../expression.h"
#include "../factory.h"

namespace {

typedef struct GlobalIndexAndDictionaryIndex {
    size_t key; // Global index
    size_t value; // Dictionary index
    bool occupied;
} GlobalIndexAndDictionaryIndex;

typedef struct DictionaryIndexTable {
    GlobalIndexAndDictionaryIndex* data;
    size_t count;
    size_t capacity;
} DictionaryIndexTable;

DictionaryIndexTable bindName(DictionaryIndexTable index_table_owner, BoundLocalName& name) {
    name.dictionary_index = SIZE_MAX;
    GET_KEY_VALUE(name.global_index, name.dictionary_index, index_table_owner);
    if (name.dictionary_index == SIZE_MAX) {
        name.dictionary_index = index_table_owner.count;
        SET_KEY_VALUE(name.global_index, name.dictionary_index, index_table_owner);
    }
    return index_table_owner;
}

// Builds the dictionary's local name table and leaves it to the caller to
// free, since `resolveDictionary` also uses it to resolve same-dictionary
// LookupSymbol/FunctionApplication/TypedExpression references.
DictionaryIndexTable bindDictionaryNames(Dictionary& dictionary_struct) {
    auto index_table_owner = DictionaryIndexTable{};

    FOR_EACH(i, dictionary_struct.statements) {
        const auto statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            auto& definition = storage.definitions.data[statement.index];
            index_table_owner = bindName(index_table_owner, definition.name);
        }
        else if (type == PUT_ASSIGNMENT) {
            auto& put_assignment = storage.put_assignments.data[statement.index];
            index_table_owner = bindName(index_table_owner, put_assignment.name);
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            auto& put_each_assignment = storage.put_each_assignments.data[statement.index];
            index_table_owner = bindName(index_table_owner, put_each_assignment.name);
        }
        else if (type == DROP_ASSIGNMENT) {
            auto& drop_assignment = storage.drop_assignments.data[statement.index];
            index_table_owner = bindName(index_table_owner, drop_assignment.name);
        }
        else if (type == FOR_STATEMENT) {
            auto& for_statement = storage.for_statements.data[statement.index];
            index_table_owner = bindName(index_table_owner, for_statement.item_name);
            index_table_owner = bindName(index_table_owner, for_statement.container_name);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            auto& for_statement = storage.for_simple_statements.data[statement.index];
            index_table_owner = bindName(index_table_owner, for_statement.container_name);
        }
    }
    dictionary_struct.definition_count = index_table_owner.count;
    return index_table_owner;
}

struct DynamicIndices {
    size_t* data;
    size_t count;
    size_t capacity;
};

// Matches each loop-end statement to its loop-start statement, using local
// indices relative to the start of the dictionary's own statement range
// (the same convention `evaluateDictionary` uses to jump between them).
// A generic END_STATEMENT is refined here into the concrete
// WHILE_END_STATEMENT/FOR_END_STATEMENT/FOR_SIMPLE_END_STATEMENT it closes.
void resolveDictionaryLoops(Dictionary& dictionary_struct) {
    const auto base_index = dictionary_struct.statements.data;
    auto loop_start_indices_owner = DynamicIndices{};

    FOR_EACH(i, dictionary_struct.statements) {
        const auto local_index = i - base_index;
        auto& statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == WHILE_STATEMENT || type == FOR_STATEMENT || type == FOR_SIMPLE_STATEMENT) {
            APPEND(loop_start_indices_owner, local_index);
        }
        else if (type == END_STATEMENT) {
            const auto start_local_index = LAST_ITEM(loop_start_indices_owner);
            DROP_BACK(loop_start_indices_owner);
            auto& start_statement = storage.statements.data[base_index + start_local_index];
            if (start_statement.type == WHILE_STATEMENT) {
                storage.while_statements.data[start_statement.index].end_index = local_index;
                statement = makeWhileEndStatement(statement.range, WhileEndStatement{start_local_index});
            } else if (start_statement.type == FOR_STATEMENT) {
                storage.for_statements.data[start_statement.index].end_index = local_index;
                statement = makeForEndStatement(statement.range, ForEndStatement{start_local_index});
            } else if (start_statement.type == FOR_SIMPLE_STATEMENT) {
                storage.for_simple_statements.data[start_statement.index].end_index = local_index;
                statement = makeForSimpleEndStatement(statement.range, ForSimpleEndStatement{start_local_index});
            }
        }
    }
    FREE_DARRAY(loop_start_indices_owner);
}

void resolveExpression(Expression expression);

// Tries to resolve a name reference against the innermost enclosing
// dictionary's own local names. On success, `parent_steps == 0` marks it
// resolvable with a direct slot lookup; on failure it is left untouched
// (still `parent_steps == -1`), so evaluation falls back to the existing
// dynamic search up the environment chain.
bool tryResolveInScope(BoundGlobalName& name, const DictionaryIndexTable& table) {
    size_t dictionary_index = SIZE_MAX;
    GET_KEY_VALUE(name.global_index, dictionary_index, table);
    if (dictionary_index == SIZE_MAX) {
        return false;
    }
    name.parent_steps = 0;
    name.dictionary_index = dictionary_index;
    return true;
}

void resolveExpressionInScope(Expression expression, const DictionaryIndexTable& table);

void resolveStatementInScope(Expression statement, const DictionaryIndexTable& table) {
    switch (statement.type) {
        case DEFINITION: return resolveExpressionInScope(storage.definitions.data[statement.index].expression, table);
        case PUT_ASSIGNMENT: return resolveExpressionInScope(storage.put_assignments.data[statement.index].expression, table);
        case PUT_EACH_ASSIGNMENT: return resolveExpressionInScope(storage.put_each_assignments.data[statement.index].expression, table);
        case WHILE_STATEMENT: return resolveExpressionInScope(storage.while_statements.data[statement.index].expression, table);
        default: return;
    }
}

void resolveConditionalInScope(Expression expression, const DictionaryIndexTable& table) {
    const auto& conditional = storage.conditionals.data[expression.index];
    FOR_EACH(i, conditional.alternatives) {
        const auto& alternative = storage.alternatives.data[i];
        resolveExpressionInScope(alternative.left, table);
        resolveExpressionInScope(alternative.right, table);
    }
    resolveExpressionInScope(conditional.expression_else, table);
}

void resolveIsInScope(Expression expression, const DictionaryIndexTable& table) {
    const auto& is_expression = storage.is_expressions.data[expression.index];
    resolveExpressionInScope(is_expression.input, table);
    FOR_EACH(i, is_expression.alternative) {
        const auto& alternative = storage.alternatives.data[i];
        resolveExpressionInScope(alternative.left, table);
        resolveExpressionInScope(alternative.right, table);
    }
    resolveExpressionInScope(is_expression.expression_else, table);
}

void resolveTupleInScope(Expression expression, const DictionaryIndexTable& table) {
    const auto& tuple = storage.tuples.data[expression.index];
    FOR_EACH(i, tuple.indices) {
        resolveExpressionInScope(storage.expressions.data[i], table);
    }
}

void resolveStackInScope(Expression expression, const DictionaryIndexTable& table) {
    auto current = expression;
    while (current.type == STACK) {
        const auto& stack = storage.stacks.data[current.index];
        resolveExpressionInScope(stack.top, table);
        current = stack.rest;
    }
}

void resolveTableInScope(Expression expression, const DictionaryIndexTable& table) {
    const auto& t = storage.tables.data[expression.index];
    FOR_EACH(i, t.rows) {
        const auto& row = storage.rows.data[i];
        resolveExpressionInScope(row.key, table);
        resolveExpressionInScope(row.value, table);
    }
}

void resolveLookupChildInScope(Expression expression, const DictionaryIndexTable& table) {
    return resolveExpressionInScope(storage.child_lookups.data[expression.index].child, table);
}

void resolveFunctionApplicationInScope(Expression expression, const DictionaryIndexTable& table) {
    auto& function_application = storage.function_applications.data[expression.index];
    tryResolveInScope(function_application.name, table);
    resolveExpressionInScope(function_application.child, table);
}

void resolveTypedExpressionInScope(Expression expression, const DictionaryIndexTable& table) {
    auto& typed_expression = storage.typed_expressions.data[expression.index];
    tryResolveInScope(typed_expression.type_name, table);
    resolveExpressionInScope(typed_expression.value, table);
}

void resolveDynamicExpressionInScope(Expression expression, const DictionaryIndexTable& table) {
    return resolveExpressionInScope(storage.dynamic_expressions.data[expression.index].expression, table);
}

void resolveLookupSymbolInScope(Expression expression, const DictionaryIndexTable& table) {
    tryResolveInScope(storage.symbol_lookups.data[expression.index].name, table);
}

// Mirrors `resolveExpression`'s traversal, but additionally attempts to
// resolve name references against `table` (the innermost dictionary's own
// locals). Stops carrying `table` at a nested DICTIONARY/FUNCTION* boundary,
// since that is a different scope, and defers to the plain (context-free)
// walker there instead.
void resolveExpressionInScope(Expression expression, const DictionaryIndexTable& table) {
    switch (expression.type) {
        case DICTIONARY: return resolveExpression(expression);
        case FUNCTION: return resolveExpression(expression);
        case FUNCTION_DICTIONARY: return resolveExpression(expression);
        case FUNCTION_TUPLE: return resolveExpression(expression);
        case CONDITIONAL: return resolveConditionalInScope(expression, table);
        case IS: return resolveIsInScope(expression, table);
        case TUPLE: return resolveTupleInScope(expression, table);
        case STACK: return resolveStackInScope(expression, table);
        case TABLE: return resolveTableInScope(expression, table);
        case LOOKUP_CHILD: return resolveLookupChildInScope(expression, table);
        case FUNCTION_APPLICATION: return resolveFunctionApplicationInScope(expression, table);
        case TYPED_EXPRESSION: return resolveTypedExpressionInScope(expression, table);
        case DYNAMIC_EXPRESSION: return resolveDynamicExpressionInScope(expression, table);
        case LOOKUP_SYMBOL: return resolveLookupSymbolInScope(expression, table);
        default: return;
    }
}

void resolveDictionary(Expression expression) {
    auto& dictionary_struct = storage.dictionaries.data[expression.index];
    auto index_table = bindDictionaryNames(dictionary_struct);
    resolveDictionaryLoops(dictionary_struct);
    FOR_EACH(i, dictionary_struct.statements) {
        resolveStatementInScope(storage.statements.data[i], index_table);
    }
    FREE_TABLE(index_table);
}

void resolveFunction(Expression expression) {
    return resolveExpression(storage.functions.data[expression.index].body);
}

void resolveFunctionDictionary(Expression expression) {
    return resolveExpression(storage.dictionary_functions.data[expression.index].body);
}

void resolveFunctionTuple(Expression expression) {
    return resolveExpression(storage.tuple_functions.data[expression.index].body);
}

void resolveConditional(Expression expression) {
    const auto& conditional = storage.conditionals.data[expression.index];
    FOR_EACH(i, conditional.alternatives) {
        const auto& alternative = storage.alternatives.data[i];
        resolveExpression(alternative.left);
        resolveExpression(alternative.right);
    }
    resolveExpression(conditional.expression_else);
}

void resolveIs(Expression expression) {
    const auto& is_expression = storage.is_expressions.data[expression.index];
    resolveExpression(is_expression.input);
    FOR_EACH(i, is_expression.alternative) {
        const auto& alternative = storage.alternatives.data[i];
        resolveExpression(alternative.left);
        resolveExpression(alternative.right);
    }
    resolveExpression(is_expression.expression_else);
}

void resolveTuple(Expression expression) {
    const auto& tuple = storage.tuples.data[expression.index];
    FOR_EACH(i, tuple.indices) {
        resolveExpression(storage.expressions.data[i]);
    }
}

void resolveStack(Expression expression) {
    auto current = expression;
    while (current.type == STACK) {
        const auto& stack = storage.stacks.data[current.index];
        resolveExpression(stack.top);
        current = stack.rest;
    }
}

void resolveTable(Expression expression) {
    const auto& table = storage.tables.data[expression.index];
    FOR_EACH(i, table.rows) {
        const auto& row = storage.rows.data[i];
        resolveExpression(row.key);
        resolveExpression(row.value);
    }
}

void resolveLookupChild(Expression expression) {
    return resolveExpression(storage.child_lookups.data[expression.index].child);
}

void resolveFunctionApplication(Expression expression) {
    return resolveExpression(storage.function_applications.data[expression.index].child);
}

void resolveTypedExpression(Expression expression) {
    return resolveExpression(storage.typed_expressions.data[expression.index].value);
}

void resolveDynamicExpression(Expression expression) {
    return resolveExpression(storage.dynamic_expressions.data[expression.index].expression);
}

void resolveExpression(Expression expression) {
    switch (expression.type) {
        case DICTIONARY: return resolveDictionary(expression);
        case FUNCTION: return resolveFunction(expression);
        case FUNCTION_DICTIONARY: return resolveFunctionDictionary(expression);
        case FUNCTION_TUPLE: return resolveFunctionTuple(expression);
        case CONDITIONAL: return resolveConditional(expression);
        case IS: return resolveIs(expression);
        case TUPLE: return resolveTuple(expression);
        case STACK: return resolveStack(expression);
        case TABLE: return resolveTable(expression);
        case LOOKUP_CHILD: return resolveLookupChild(expression);
        case FUNCTION_APPLICATION: return resolveFunctionApplication(expression);
        case TYPED_EXPRESSION: return resolveTypedExpression(expression);
        case DYNAMIC_EXPRESSION: return resolveDynamicExpression(expression);
        // Everything else (LOOKUP_SYMBOL, NUMBER, CHARACTER, strings, YES/NO,
        // ARGUMENT, EMPTY_STACK, ERROR_EXPRESSION, ...) is a leaf: nothing to recurse into.
        default: return;
    }
}

} // namespace

Expression resolve(Expression expression) {
    resolveExpression(expression);
    return expression;
}
