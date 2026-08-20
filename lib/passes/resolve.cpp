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

void bindDictionaryNames(Dictionary& dictionary_struct) {
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
    FREE_TABLE(index_table_owner);
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

// Recurses into a dictionary statement's own expression field(s), if it has
// any, so nested dictionaries/functions/etc. reachable from it get visited.
void resolveStatement(Expression statement) {
    const auto type = statement.type;
    if (type == DEFINITION) {
        resolveExpression(storage.definitions.data[statement.index].expression);
    }
    else if (type == PUT_ASSIGNMENT) {
        resolveExpression(storage.put_assignments.data[statement.index].expression);
    }
    else if (type == PUT_EACH_ASSIGNMENT) {
        resolveExpression(storage.put_each_assignments.data[statement.index].expression);
    }
    else if (type == WHILE_STATEMENT) {
        resolveExpression(storage.while_statements.data[statement.index].expression);
    }
    // DROP_ASSIGNMENT, FOR_STATEMENT, FOR_SIMPLE_STATEMENT, RETURN_STATEMENT,
    // and the various end-statements carry only names/indices, no expression.
}

void resolveExpression(Expression expression) {
    const auto type = expression.type;
    if (type == DICTIONARY) {
        auto& dictionary_struct = storage.dictionaries.data[expression.index];
        bindDictionaryNames(dictionary_struct);
        resolveDictionaryLoops(dictionary_struct);
        FOR_EACH(i, dictionary_struct.statements) {
            resolveStatement(storage.statements.data[i]);
        }
    }
    else if (type == FUNCTION) {
        resolveExpression(storage.functions.data[expression.index].body);
    }
    else if (type == FUNCTION_DICTIONARY) {
        resolveExpression(storage.dictionary_functions.data[expression.index].body);
    }
    else if (type == FUNCTION_TUPLE) {
        resolveExpression(storage.tuple_functions.data[expression.index].body);
    }
    else if (type == CONDITIONAL) {
        const auto& conditional = storage.conditionals.data[expression.index];
        FOR_EACH(i, conditional.alternatives) {
            const auto& alternative = storage.alternatives.data[i];
            resolveExpression(alternative.left);
            resolveExpression(alternative.right);
        }
        resolveExpression(conditional.expression_else);
    }
    else if (type == IS) {
        const auto& is_expression = storage.is_expressions.data[expression.index];
        resolveExpression(is_expression.input);
        FOR_EACH(i, is_expression.alternative) {
            const auto& alternative = storage.alternatives.data[i];
            resolveExpression(alternative.left);
            resolveExpression(alternative.right);
        }
        resolveExpression(is_expression.expression_else);
    }
    else if (type == TUPLE) {
        const auto& tuple = storage.tuples.data[expression.index];
        FOR_EACH(i, tuple.indices) {
            resolveExpression(storage.expressions.data[i]);
        }
    }
    else if (type == STACK) {
        auto current = expression;
        while (current.type == STACK) {
            const auto& stack = storage.stacks.data[current.index];
            resolveExpression(stack.top);
            current = stack.rest;
        }
    }
    else if (type == TABLE) {
        const auto& table = storage.tables.data[expression.index];
        FOR_EACH(i, table.rows) {
            const auto& row = storage.rows.data[i];
            resolveExpression(row.key);
            resolveExpression(row.value);
        }
    }
    else if (type == LOOKUP_CHILD) {
        resolveExpression(storage.child_lookups.data[expression.index].child);
    }
    else if (type == FUNCTION_APPLICATION) {
        resolveExpression(storage.function_applications.data[expression.index].child);
    }
    else if (type == TYPED_EXPRESSION) {
        resolveExpression(storage.typed_expressions.data[expression.index].value);
    }
    else if (type == DYNAMIC_EXPRESSION) {
        resolveExpression(storage.dynamic_expressions.data[expression.index].expression);
    }
    // Everything else (LOOKUP_SYMBOL, NUMBER, CHARACTER, strings, YES/NO,
    // ARGUMENT, EMPTY_STACK, ERROR_EXPRESSION, ...) is a leaf: nothing to recurse into.
}

} // namespace

Expression resolve(Expression expression) {
    resolveExpression(expression);
    return expression;
}
