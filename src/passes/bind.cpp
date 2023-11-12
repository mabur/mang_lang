#include "bind.h"

#include <unordered_map>

#include "../expression.h"
#include "../factory.h"


namespace {

struct DictionaryNameIndexer {
    std::unordered_map<size_t, size_t> dictionary_index_from_global_index;
    size_t count = 0;

    size_t getDictionaryIndex(size_t name) {
        const auto it = dictionary_index_from_global_index.find(name);
        if (it != dictionary_index_from_global_index.end()) {
            return it->second;
        }
        else {
            dictionary_index_from_global_index[name] = count;
            return count++;
        }
    }
};

void bindFunctionDictionary(Expression function_dictionary, Expression environment) {
    const auto function_dictionary_struct = storage.dictionary_functions.at(function_dictionary.index);
    // TODO: add arguments to environment.
    bind(function_dictionary_struct.body, environment);
}

void bindFunctionTuple(Expression function_tuple, Expression environment) {
    const auto function_tuple_struct = storage.tuple_functions.at(function_tuple.index);
    // TODO: add arguments to environment.
    bind(function_tuple_struct.body, environment);
}

void bindFunction(Expression function, Expression environment) {
    const auto function_struct = storage.functions.at(function.index);
    // TODO: add argument to environment.
    bind(function_struct.body, environment);
}

void bindLookupSymbol(Expression expression, Expression environment) {
    (void)expression;
    (void)environment;
    // TODO: resolve and bind name.
}

void bindStack(Expression stack, Expression environment) {
    while (stack.type != EMPTY_STACK) {
        if (stack.type != STACK) {
            throw std::runtime_error(
                std::string{"\n\nI have found a static type error."} +
                    "\nIt happens in bindStack. " +
                    "\nInstead of a stack I got a " + NAMES[stack.type] +
                    ".\n"
            );
        }
        const auto stack_struct = storage.stacks.at(stack.index);
        const auto top = stack_struct.top;
        const auto rest = stack_struct.rest;
        bind(top, environment);
        stack = rest;
    }
}

void bindTuple(Expression tuple, Expression environment) {
    const auto tuple_struct = storage.tuples.at(tuple.index);
    for (auto& expression : tuple_struct.expressions) {
        bind(expression, environment);
    }
}

void bindTable(Expression table, Expression environment) {
    const auto table_struct = storage.tables.at(table.index);
    for (const auto& row : table_struct.rows) {
        bind(row.key, environment);
        bind(row.value, environment);
    }
}

void bindLookupChild(Expression lookup_child, Expression environment) {
    const auto lookup_child_struct = storage.child_lookups.at(lookup_child.index);
    bind(lookup_child_struct.child, environment);
}

void bindTypedExpression(Expression expression, Expression environment) {
    const auto expression_struct = storage.typed_expressions.at(expression.index);
    // TODO: resolve type_name and bind it.
    bind(expression_struct.value, environment);
}

void bindDynamicExpression(Expression expression, Expression environment) {
    const auto inner_expression = storage.dynamic_expressions.at(expression.index).expression;
    bind(inner_expression, environment);
}

void bindConditional(Expression conditional, Expression environment) {
    const auto conditional_struct = storage.conditionals.at(conditional.index);
    for (auto a = conditional_struct.alternative_first;
        a.index <= conditional_struct.alternative_last.index;
        ++a.index
        ) {
        const auto alternative = storage.alternatives.at(a.index);
        bind(alternative.left, environment);
        bind(alternative.right, environment);
    }
    bind(conditional_struct.expression_else, environment);
}

void bindIs(Expression is, Expression environment) {
    const auto is_struct = storage.is_expressions.at(is.index);
    bind(is_struct.input, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
        ) {
        const auto alternative = storage.alternatives.at(a.index);
        bind(alternative.left, environment);
        bind(alternative.right, environment);
    }
    bind(is_struct.expression_else, environment);
}

void bindFunctionApplication(
    Expression function_application, Expression environment
) {
    const auto function_application_struct = storage.function_applications.at(function_application.index);
    // TODO: resolve and bind function name.
    bind(function_application_struct.child, environment);
}

} // namespace

void bind(Expression expression, Expression environment) {
    switch (expression.type) {
        case NUMBER: return;
        case CHARACTER: return;
        case YES: return;
        case NO: return;
        case EMPTY_STRING: return;
        case STRING: return;
        case EMPTY_STACK: return;
        
        case FUNCTION: return bindFunction(expression, environment);
        case FUNCTION_TUPLE: return bindFunctionTuple(expression, environment);
        case FUNCTION_DICTIONARY: return bindFunctionDictionary(expression, environment);
        case LOOKUP_SYMBOL: return bindLookupSymbol(expression, environment);
        
        case STACK: return bindStack(expression, environment);
        case TUPLE: return bindTuple(expression, environment);
        case TABLE: return bindTable(expression, environment);
        case LOOKUP_CHILD: return bindLookupChild(expression, environment);
        case TYPED_EXPRESSION: return bindTypedExpression(expression, environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return bindDynamicExpression(expression, environment);
        case CONDITIONAL: return bindConditional(expression, environment);
        case IS: return bindIs(expression, environment);
        case DICTIONARY: return; // TODO
        case FUNCTION_APPLICATION: return bindFunctionApplication(expression, environment);

        default: throw UnexpectedExpression(expression.type, "bind operation");
    }
}
