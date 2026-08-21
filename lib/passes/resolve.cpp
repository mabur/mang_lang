#include "resolve.h"

#include <carma/carma.h>

#include "../expression.h"
#include "../factory.h"

namespace {

struct DynamicIndices {
    size_t* data;
    size_t count;
    size_t capacity;
};
    
DynamicIndices bindLocalName(DynamicIndices seen_names_owner, BoundLocalName* name) {
    FOR_INDEX(i, seen_names_owner) {
        if (seen_names_owner.data[i] == name->global_index) {
            name->dictionary_index = i;
            return seen_names_owner;
        }
    }
    name->dictionary_index = seen_names_owner.count;
    APPEND(seen_names_owner, name->global_index);
    return seen_names_owner;
}

DynamicIndices bindNameForStatement(DynamicIndices seen_names_owner, Expression statement) {
    seen_names_owner = bindLocalName(seen_names_owner, &storage.for_statements.data[statement.index].item_name);
    seen_names_owner = bindLocalName(seen_names_owner, &storage.for_statements.data[statement.index].container_name);
    return seen_names_owner;
}

DynamicIndices bindNameStatement(DynamicIndices seen_names_owner, Expression statement) {
    switch (statement.type) {
        case DEFINITION: return bindLocalName(seen_names_owner, &storage.definitions.data[statement.index].name);
        case PUT_ASSIGNMENT: return bindLocalName(seen_names_owner, &storage.put_assignments.data[statement.index].name);
        case PUT_EACH_ASSIGNMENT: return bindLocalName(seen_names_owner, &storage.put_each_assignments.data[statement.index].name);
        case DROP_ASSIGNMENT: return bindLocalName(seen_names_owner, &storage.drop_assignments.data[statement.index].name);
        case FOR_STATEMENT: return bindNameForStatement(seen_names_owner, statement);
        case FOR_SIMPLE_STATEMENT: return bindLocalName(seen_names_owner, &storage.for_simple_statements.data[statement.index].container_name);
        default: return seen_names_owner;
    }
}

// Matches each loop-end statement to its loop-start statement, using local
// indices relative to the start of the dictionary's own statement range
// (the same convention `evaluateDictionary` uses to jump between them).
// A generic END_STATEMENT is refined here into the concrete
// WHILE_END_STATEMENT/FOR_END_STATEMENT/FOR_SIMPLE_END_STATEMENT it closes.
void resolveDictionaryLoops(Dictionary dictionary_struct) {
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

OptionalIndex findInForStatement(ForStatement for_statement, size_t global_index) {
    auto result = matchName(for_statement.item_name, global_index);
    if (result.ok) return result;
    return matchName(for_statement.container_name, global_index);
}

OptionalIndex findInDictionaryInner(Expression statement, size_t global_index) {
    switch (statement.type) {
    case DEFINITION: return matchName(storage.definitions.data[statement.index].name, global_index);
    case PUT_ASSIGNMENT: return matchName(storage.put_assignments.data[statement.index].name, global_index);
    case PUT_EACH_ASSIGNMENT: return matchName(storage.put_each_assignments.data[statement.index].name, global_index);
    case DROP_ASSIGNMENT: return matchName(storage.drop_assignments.data[statement.index].name, global_index);
    case FOR_STATEMENT: return findInForStatement(storage.for_statements.data[statement.index], global_index);
    case FOR_SIMPLE_STATEMENT: return matchName(storage.for_simple_statements.data[statement.index].container_name, global_index);
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

// Tries to resolve a name reference against the chain of lexically
// enclosing scopes, walking outward and counting hops, innermost first (so
// an inner declaration correctly shadows an outer one of the same name).
// On success, `parent_steps`/`dictionary_index` mark it resolvable with a
// direct hop-and-index lookup; on failure it is returned untouched (still
// `parent_steps == -1`), so evaluation falls back to the existing dynamic
// search up the environment chain.
BoundGlobalName tryResolve(BoundGlobalName name, ScopeChain chain) {
    for (auto steps = 0; ;++steps, chain = *chain.parent) {
        auto result = findInScope(chain.scope, name.global_index);
        if (result.ok) {
            name.parent_steps = steps;
            name.dictionary_index = result.index;
            return name;
        }
        if (chain.parent == nullptr) {
            return name;
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
        // DROP_ASSIGNMENT, FOR_STATEMENT, FOR_SIMPLE_STATEMENT, RETURN_STATEMENT,
        // and the various end-statements carry only names/indices, no expression.
        default: return;
    }
}

void resolveDictionary(Expression expression, ScopeChain chain) {
    auto dictionary_struct = &storage.dictionaries.data[expression.index];
    
    auto seen_names_owner = DynamicIndices{};
    FOR_EACH(i, dictionary_struct->statements) {
        seen_names_owner = bindNameStatement(seen_names_owner, storage.statements.data[i]);
    }
    dictionary_struct->definition_count = seen_names_owner.count;
    FREE_DARRAY(seen_names_owner);
    
    resolveDictionaryLoops(*dictionary_struct);
    FOR_EACH(i, dictionary_struct->statements) {
        resolveStatement(storage.statements.data[i], ScopeChain{expression, &chain});
    }
}

// Argument type annotations (the `Number` in `Number:x`) are resolved
// against the function's own enclosing scope, not its own argument scope:
// checkArgument() evaluates them against function_struct.environment, so
// an annotation can never refer to a sibling argument.
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
    function_application.name = tryResolve(function_application.name, chain);
    resolveExpression(function_application.child, chain);
}

void resolveBuiltInApplication(Expression expression, ScopeChain chain) {
    return resolveExpression(storage.function_applications_built_in.data[expression.index].child, chain);
}

void resolveTypedExpression(Expression expression, ScopeChain chain) {
    auto& typed_expression = storage.typed_expressions.data[expression.index];
    typed_expression.type_name = tryResolve(typed_expression.type_name, chain);
    resolveExpression(typed_expression.value, chain);
}

void resolveDynamicExpression(Expression expression, ScopeChain chain) {
    return resolveExpression(storage.dynamic_expressions.data[expression.index].expression, chain);
}

void resolveLookupSymbol(Expression expression, ScopeChain chain) {
    storage.symbol_lookups.data[expression.index].name = tryResolve(storage.symbol_lookups.data[expression.index].name, chain);
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
