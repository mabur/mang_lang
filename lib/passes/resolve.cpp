#include "resolve.h"

#include <carma/carma.h>

#include "../expression.h"
#include "../factory.h"
#include "../built_in_functions/built_in_functions.h"

namespace {

struct DynamicIndices {
    size_t* data;
    size_t count;
    size_t capacity;
};
    
DynamicIndices bindLocalName(DynamicIndices dictionary_names_owner, BoundLocalName* name) {
    FOR_INDEX(i, dictionary_names_owner) {
        if (dictionary_names_owner.data[i] == name->global_index) {
            name->dictionary_index = i;
            return dictionary_names_owner;
        }
    }
    name->dictionary_index = dictionary_names_owner.count;
    APPEND(dictionary_names_owner, name->global_index);
    return dictionary_names_owner;
}

DynamicIndices bindLocalNameStatement(DynamicIndices dictionary_names_owner, Expression statement) {
    switch (statement.type) {
        case DEFINITION: return bindLocalName(dictionary_names_owner, &storage.definitions.data[statement.index].name);
        case PUT_ASSIGNMENT: return bindLocalName(dictionary_names_owner, &storage.put_assignments.data[statement.index].name);
        case PUT_EACH_ASSIGNMENT: return bindLocalName(dictionary_names_owner, &storage.put_each_assignments.data[statement.index].name);
        case DROP_ASSIGNMENT: return bindLocalName(dictionary_names_owner, &storage.drop_assignments.data[statement.index].name);
        case FOR_INIT_STATEMENT: return bindLocalName(dictionary_names_owner, &storage.for_init_statements.data[statement.index].name);
        case FOR_STATEMENT: return bindLocalName(dictionary_names_owner, &storage.for_statements.data[statement.index].name);
        default: return dictionary_names_owner;
    }
}

void resolveDictionaryLoops(Dictionary dictionary_struct) {
    const auto base_index = dictionary_struct.statements.data;
    auto loop_start_indices_owner = DynamicIndices{};

    FOR_EACH(i, dictionary_struct.statements) {
        const auto local_index = i - base_index;
        auto statement = &storage.statements.data[i];
        const auto type = statement->type;
        if (type == WHILE_STATEMENT || type == FOR_STATEMENT || type == IF_STATEMENT) {
            APPEND(loop_start_indices_owner, local_index);
        }
        else if (type == END_STATEMENT) {
            const auto start_local_index = LAST_ITEM(loop_start_indices_owner);
            DROP_BACK(loop_start_indices_owner);
            auto& start_statement = storage.statements.data[base_index + start_local_index];
            if (start_statement.type == WHILE_STATEMENT) {
                storage.while_statements.data[start_statement.index].end_index = local_index;
                *statement = makeWhileEndStatement(statement->range, WhileEndStatement{start_local_index});
            } else if (start_statement.type == FOR_STATEMENT) {
                storage.for_statements.data[start_statement.index].end_index = local_index;
                *statement = makeForEndStatement(statement->range, ForEndStatement{start_local_index});
            } else if (start_statement.type == IF_STATEMENT) {
                storage.if_statements.data[start_statement.index].end_index = local_index;
                *statement = Expression{0, statement->range, IF_END_STATEMENT};
            }
        }
    }
    FREE_DARRAY(loop_start_indices_owner);
}

// A single link in the chain of lexically enclosing scopes the resolver is
// currently nested inside, innermost first. `scope` is a DICTIONARY,
// FUNCTION, FUNCTION_DICTIONARY, or FUNCTION_TUPLE expression, or the
// default Expression{} (type ANY) to mean "no scope here". `parent` is
// whatever lexically encloses it (nullptr if nothing further is known).
// Each link lives only for the duration of the recursive resolve() call
// that built it, the same as any other stack-allocated recursive-descent
// state - it never needs to outlive that call.
struct ScopeChain {
    Expression scope;
    const ScopeChain* parent;
};

struct OptionalIndex {
    size_t index;
    bool ok;
};

// Looks for `global_index` among the arguments listed in `arguments`
// (indices into storage.arguments), returning its declaration-order
// position. Shared by FUNCTION_DICTIONARY and FUNCTION_TUPLE, and by
// FUNCTION via a single-element Indices.
OptionalIndex findInArguments(Indices arguments, size_t global_index) {
    FOR_EACH(i, arguments) {
        if (storage.arguments.data[i].name == global_index) {
            return OptionalIndex{i - BEGIN_POINTER(arguments), true};
        }
    }
    return OptionalIndex{};
}

OptionalIndex matchName(BoundLocalName name, size_t global_index) {
    return name.global_index == global_index ? OptionalIndex{name.dictionary_index, true} : OptionalIndex{};
}

OptionalIndex findInDictionaryInner(Expression statement, size_t global_index) {
    switch (statement.type) {
    case DEFINITION: return matchName(storage.definitions.data[statement.index].name, global_index);
    case PUT_ASSIGNMENT: return matchName(storage.put_assignments.data[statement.index].name, global_index);
    case PUT_EACH_ASSIGNMENT: return matchName(storage.put_each_assignments.data[statement.index].name, global_index);
    case DROP_ASSIGNMENT: return matchName(storage.drop_assignments.data[statement.index].name, global_index);
    case FOR_INIT_STATEMENT: return matchName(storage.for_init_statements.data[statement.index].name, global_index);
    case FOR_STATEMENT: return matchName(storage.for_statements.data[statement.index].name, global_index);
    default: return OptionalIndex{};
    }
}

// Looks for `global_index` among the locally-bound names in this
// dictionary's own statements, reading the dictionary_index each was
// already assigned by bindDictionaryNames.
OptionalIndex findInDictionary(Expression dictionary_expression, size_t global_index) {
    auto dictionary_struct = storage.dictionaries.data[dictionary_expression.index];
    FOR_EACH(i, dictionary_struct.statements) {
        auto statement = storage.statements.data[i];
        auto result = findInDictionaryInner(statement, global_index);
        if (result.ok) return result;
    }
    return OptionalIndex{};
}

// Looks for `global_index` directly within one scope, ignoring its parents.
// A scope of type ANY (the empty sentinel) safely falls through to not-found.
OptionalIndex findInScope(Expression scope, size_t global_index) {
    switch (scope.type) {
        case DICTIONARY: return findInDictionary(scope, global_index);
        case FUNCTION: return findInArguments(Indices{storage.functions.data[scope.index].argument, 1}, global_index);
        case FUNCTION_DICTIONARY: return findInArguments(storage.dictionary_functions.data[scope.index].arguments, global_index);
        case FUNCTION_TUPLE: return findInArguments(storage.tuple_functions.data[scope.index].arguments, global_index);
        default: return OptionalIndex{};
    }
}

BoundGlobalName tryBindGlobalName(BoundGlobalName name, ScopeChain chain) {
    for (auto steps = 0; ;++steps, chain = *chain.parent) {
        auto result = findInScope(chain.scope, name.global_index);
        if (result.ok) {
            name.parent_steps = steps;
            name.dictionary_index = result.index;
            return name;
        }
        if (chain.parent == nullptr) {
            auto built_in = findBuiltIn(name.global_index);
            if (built_in != nullptr) {
                name.parent_steps = chain.scope.type == ANY ? steps : steps + 1;
                name.dictionary_index = built_in - BUILT_IN_ENTRIES;
            }
            return name; // Error, but report it during evaluation for now.
        }
    }
}

void resolveExpression(Expression expression, ScopeChain chain);

void resolveStatement(Expression statement, ScopeChain chain) {
    switch (statement.type) {
        case DEFINITION: return resolveExpression(storage.definitions.data[statement.index].expression, chain);
        case PUT_ASSIGNMENT: return resolveExpression(storage.put_assignments.data[statement.index].expression, chain);
        case PUT_EACH_ASSIGNMENT: return resolveExpression(storage.put_each_assignments.data[statement.index].expression, chain);
        case WHILE_STATEMENT: return resolveExpression(storage.while_statements.data[statement.index].expression, chain);
        case IF_STATEMENT: return resolveExpression(storage.if_statements.data[statement.index].expression, chain);
        case FOR_INIT_STATEMENT: return resolveExpression(storage.for_init_statements.data[statement.index].container_expression, chain);
        default: return;
    }
}

void resolveDictionary(Expression expression, ScopeChain chain) {
    auto dictionary_struct = &storage.dictionaries.data[expression.index];
    
    auto dictionary_names_owner = DynamicIndices{};
    FOR_EACH(i, dictionary_struct->statements) {
        dictionary_names_owner = bindLocalNameStatement(dictionary_names_owner, storage.statements.data[i]);
    }
    dictionary_struct->definition_count = dictionary_names_owner.count;
    FREE_DARRAY(dictionary_names_owner);
    
    resolveDictionaryLoops(*dictionary_struct);
    FOR_EACH(i, dictionary_struct->statements) {
        resolveStatement(storage.statements.data[i], ScopeChain{expression, &chain});
    }
}

void resolveArgumentTypes(Indices arguments, ScopeChain chain) {
    FOR_EACH(i, arguments) {
        auto argument = storage.arguments.data[i];
        if (argument.type.type != ANY) {
            resolveExpression(argument.type, chain);
        }
    }
}

void resolveFunction(Expression expression, ScopeChain chain) {
    auto function_struct = storage.functions.data[expression.index];
    resolveArgumentTypes(Indices{function_struct.argument, 1}, chain);
    resolveExpression(function_struct.body, ScopeChain{expression, &chain});
}

void resolveFunctionDictionary(Expression expression, ScopeChain chain) {
    auto function_struct = storage.dictionary_functions.data[expression.index];
    resolveArgumentTypes(function_struct.arguments, chain);
    resolveExpression(function_struct.body, ScopeChain{expression, &chain});
}

void resolveFunctionTuple(Expression expression, ScopeChain chain) {
    auto function_struct = storage.tuple_functions.data[expression.index];
    resolveArgumentTypes(function_struct.arguments, chain);
    resolveExpression(function_struct.body, ScopeChain{expression, &chain});
}

void resolveConditional(Expression expression, ScopeChain chain) {
    auto conditional = storage.conditionals.data[expression.index];
    FOR_EACH(i, conditional.alternatives) {
        auto alternative = storage.alternatives.data[i];
        resolveExpression(alternative.left, chain);
        resolveExpression(alternative.right, chain);
    }
    resolveExpression(conditional.expression_else, chain);
}

void resolveIs(Expression expression, ScopeChain chain) {
    auto is_expression = storage.is_expressions.data[expression.index];
    resolveExpression(is_expression.input, chain);
    FOR_EACH(i, is_expression.alternative) {
        auto alternative = storage.alternatives.data[i];
        resolveExpression(alternative.left, chain);
        resolveExpression(alternative.right, chain);
    }
    resolveExpression(is_expression.expression_else, chain);
}

void resolveTuple(Expression expression, ScopeChain chain) {
    auto tuple = storage.tuples.data[expression.index];
    FOR_EACH(i, tuple.indices) {
        resolveExpression(storage.expressions.data[i], chain);
    }
}

void resolveStack(Expression expression, ScopeChain chain) {
    auto current = expression;
    while (current.type == STACK) {
        auto stack = storage.stacks.data[current.index];
        resolveExpression(stack.top, chain);
        current = stack.rest;
    }
}

void resolveTable(Expression expression, ScopeChain chain) {
    auto table = storage.tables.data[expression.index];
    FOR_EACH(i, table.rows) {
        auto row = storage.rows.data[i];
        resolveExpression(row.key, chain);
        resolveExpression(row.value, chain);
    }
}

void resolveLookupChild(Expression expression, ScopeChain chain) {
    return resolveExpression(storage.child_lookups.data[expression.index].child, chain);
}

void resolveFunctionApplication(Expression expression, ScopeChain chain) {
    auto& function_application = storage.function_applications.data[expression.index];
    function_application.name = tryBindGlobalName(function_application.name, chain);
    resolveExpression(function_application.child, chain);
}

void resolveBuiltInApplication(Expression expression, ScopeChain chain) {
    return resolveExpression(storage.function_applications_built_in.data[expression.index].child, chain);
}

void resolveTypedExpression(Expression expression, ScopeChain chain) {
    auto& typed_expression = storage.typed_expressions.data[expression.index];
    resolveExpression(typed_expression.type, chain);
    resolveExpression(typed_expression.value, chain);
}

void resolveDynamicExpression(Expression expression, ScopeChain chain) {
    return resolveExpression(storage.dynamic_expressions.data[expression.index].expression, chain);
}

void resolveLookupSymbol(Expression expression, ScopeChain chain) {
    storage.symbol_lookups.data[expression.index].name = tryBindGlobalName(storage.symbol_lookups.data[expression.index].name, chain);
}

void resolveExpression(Expression expression, ScopeChain chain) {
    switch (expression.type) {
        case DICTIONARY: return resolveDictionary(expression, chain);
        case FUNCTION: return resolveFunction(expression, chain);
        case FUNCTION_DICTIONARY: return resolveFunctionDictionary(expression, chain);
        case FUNCTION_TUPLE: return resolveFunctionTuple(expression, chain);
        case CONDITIONAL: return resolveConditional(expression, chain);
        case IS: return resolveIs(expression, chain);
        case TUPLE: return resolveTuple(expression, chain);
        case STACK: return resolveStack(expression, chain);
        case TABLE: return resolveTable(expression, chain);
        case LOOKUP_CHILD: return resolveLookupChild(expression, chain);
        case FUNCTION_APPLICATION: return resolveFunctionApplication(expression, chain);
        case FUNCTION_APPLICATION_BUILT_IN: return resolveBuiltInApplication(expression, chain);
        case TYPED_EXPRESSION: return resolveTypedExpression(expression, chain);
        case DYNAMIC_EXPRESSION: return resolveDynamicExpression(expression, chain);
        case LOOKUP_SYMBOL: return resolveLookupSymbol(expression, chain);
        // Everything else (NUMBER, CHARACTER, strings, YES/NO, ARGUMENT,
        // EMPTY_STACK, ERROR_EXPRESSION, ...) is a leaf: nothing to recurse into.
        default: return;
    }
}

} // namespace

Expression resolve(Expression expression, Expression outer_scope) {
    resolveExpression(expression, ScopeChain{outer_scope, nullptr});
    return expression;
}
