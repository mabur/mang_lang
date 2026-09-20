#include "evaluate.h"

#include <cassert>
#include <string.h>

#include <carma/carma.h>

#include "../built_in_functions/container.h"
#include "../exceptions.h"
#include "../factory.h"
#include "../mang_lang_string.h"
#include "../type_check.h"
#include "serialize.h"

struct OptionalLookup {
    Expression value;
    bool ok;
};
    
static
OptionalLookup optionalLookup(DictionaryValue dictionary, size_t name) {
    auto result = MAKE(OptionalLookup);
    FOR_EACH2(name_index, slot_index, dictionary.names, dictionary.slot_values) {
        if (storage.slot_names.data[name_index] == name) {
            result.value = storage.slot_values.data[slot_index];
            result.ok = true;
        }
    }
    return result;
}

static
Expression requiredLookup(DictionaryValue dictionary, size_t name) {
    const auto result = optionalLookup(dictionary, name);
    if (result.ok) {
        return result.value;
    }
    auto name_c = storage.names.data + name;
    return makeErrorValue({}, "I cannot find name %s in dictionary", name_c);
}
    
static
TypeCheck checkTypes(Expression super, Expression sub, const char* description);

static
TypeCheck checkTypesStackValue(Expression super, Expression sub, const char* description) {
    const auto stack_super = storage.stack_values.data[super.index].top;
    const auto stack_sub = storage.stack_values.data[sub.index].top;
    return checkTypes(stack_super, stack_sub, description);
}

static
TypeCheck checkTypesTableValue(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto& table_super = storage.table_values.at(super.index);
    const auto& table_sub = storage.table_values.at(sub.index);
    if (table_super.empty()) return result;
    if (table_sub.empty()) return result;
    const auto row_super = table_super.begin()->second;
    const auto row_sub = table_sub.begin()->second;
    result = checkTypes(row_super.key, row_sub.key, description);
    if (!result.ok) return result;
    result = checkTypes(row_super.value, row_sub.value, description);
    return result;
}

static
TypeCheck checkTypesTupleValue(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto tuple_super = storage.tuple_values.data[super.index];
    const auto tuple_sub = storage.tuple_values.data[sub.index];
    const auto super_count = tuple_super.indices.count;
    const auto sub_count = tuple_sub.indices.count;
    if (super_count != sub_count) {
        result.ok = false;
        result.error = makeErrorValue({},
            "Static type error in %s. Inconsistent tuple size.", description
        );
        return result;
    }
    FOR_EACH2(super_index, sub_index, tuple_super.indices, tuple_sub.indices) {
        const auto super_expression = storage.expressions.data[super_index];
        const auto sub_expression = storage.expressions.data[sub_index];
        result = checkTypes(super_expression, sub_expression, description);
        if (!result.ok) return result;
    }
    return result;
}

static
TypeCheck checkTypesDictionaryValue(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto dictionary_super = storage.dictionary_values.data[super.index];
    const auto dictionary_sub = storage.dictionary_values.data[sub.index];
    FOR_EACH2(name_index, slot_index, dictionary_super.names, dictionary_super.slot_values) {
        const auto name_super = storage.slot_names.data[name_index];
        const auto value_super = storage.slot_values.data[slot_index];
        const auto value_sub = optionalLookup(dictionary_sub, name_super);
        if (value_sub.ok) {
            result = checkTypes(value_super, value_sub.value, description);
            if (!result.ok) return result;
        }
        else {
            result.ok = false;
            result.error = makeErrorValue({},
                "Static type error in %s. Could not find name %s in dictionary %s",
                description,
                storage.names.data + name_super,
                describeLocation(sub.range).data
            );
            return result;
        }
    }
    return result;
}

static
ExpressionType functionType(Expression expression) {
    return expression.type == FUNCTION_VALUE
        ? storage.function_values.data[expression.index].function.type
        : expression.type;
}

static
TypeCheck checkTypes(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    if (super.type == ANY_VALUE || sub.type == ANY_VALUE) return result;

    if (super.type == NUMBER && sub.type == NUMBER) return result;
    if (super.type == CHARACTER && sub.type == CHARACTER) return result;

    if (super.type == NO && sub.type == NO) return result;
    if (super.type == YES && sub.type == YES) return result;
    if (super.type == YES && sub.type == NO) return result;
    if (super.type == NO && sub.type == YES) return result;

    auto super_type = functionType(super);
    auto sub_type = functionType(sub);
    if (super_type == FUNCTION_EXPRESSION && sub_type == FUNCTION_EXPRESSION) return result;
    if (super_type == FUNCTION_EXPRESSION && sub_type == FUNCTION_DICTIONARY_EXPRESSION) return result;
    if (super_type == FUNCTION_EXPRESSION && sub_type == FUNCTION_TUPLE_EXPRESSION) return result;
    if (super_type == FUNCTION_EXPRESSION && sub_type == FUNCTION_BUILT_IN_VALUE) return result;
    if (super_type == FUNCTION_DICTIONARY_EXPRESSION && sub_type == FUNCTION_EXPRESSION) return result;
    if (super_type == FUNCTION_TUPLE_EXPRESSION && sub_type == FUNCTION_EXPRESSION) return result;
    if (super_type == FUNCTION_BUILT_IN_VALUE && sub_type == FUNCTION_EXPRESSION) return result;

    if (super.type == EMPTY_STRING && sub.type == EMPTY_STRING) return result;
    if (super.type == EMPTY_STRING && sub.type == STRING) return result;
    if (super.type == STRING && sub.type == EMPTY_STRING) return result;
    if (super.type == STRING && sub.type == STRING) return result;
    
    if (super.type == EMPTY_STACK && sub.type == EMPTY_STACK) return result;
    if (super.type == EMPTY_STACK && sub.type == STACK_VALUE) return result;
    if (super.type == STACK_VALUE && sub.type == EMPTY_STACK) return result;
    if (super.type == STACK_VALUE && sub.type == STACK_VALUE) {
        return checkTypesStackValue(super, sub, description);
    }
    if (super.type == TABLE_VALUE && sub.type == TABLE_VALUE) {
        return checkTypesTableValue(super, sub, description);
    }
    if (super.type == TUPLE_VALUE && sub.type == TUPLE_VALUE) {
        return checkTypesTupleValue(super, sub, description);
    }
    if (super.type == DICTIONARY_VALUE && sub.type == DICTIONARY_VALUE) {
        return checkTypesDictionaryValue(super, sub, description);
    }
    result.ok = false;
    result.error = makeErrorValue({},
        "Static type error in %s at %s. %s is not a supertype for %s",
        description,
        describeLocation(super.range).data,
        getFunctionTypeName(super_type),
        getFunctionTypeName(sub_type)
    );
    return result;
}

template<typename Evaluator>
static
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    // Allocation:
    auto items = Expressions{};
    while (stack.type != EMPTY_STACK) {
        if (stack.type != STACK_EXPRESSION) {
            return makeErrorValue(stack.range,
                "\n\nI have found a type error.\n"
                "It happens in evaluateStack.\n"
                "Instead of a stack I got a %s.\n",
                getExpressionName(stack.type));
        }
        const auto stack_struct = storage.stack_expressions.data[stack.index];
        const auto top = stack_struct.top;
        const auto rest = stack_struct.rest;
        APPEND(items, evaluator(top, environment));
        stack = rest;
    }
    auto evaluated_stack = Expression{0, stack.range, EMPTY_STACK};
    FOR_EACH_BACKWARD(it, items) {
        evaluated_stack = builtInPutStackValue(evaluated_stack, *it);
    }
    FREE_DARRAY(items);
    return evaluated_stack;
}

template<typename Evaluator>
static
Expression evaluateTuple(
    Evaluator evaluator, Expression tuple, Expression environment
) {
    auto tuple_struct = storage.tuple_expressions.data[tuple.index];
    auto tuple_count = tuple_struct.indices.count;
    auto first = storage.expressions.count;
    auto target_indices = Indices{first, tuple_count};
    // Allocation:
    FOR_EACH(i, target_indices) {
        APPEND(storage.expressions, Expression{});
    }
    FOR_EACH2(target_index, source_index, target_indices, tuple_struct.indices) {
        auto expression = storage.expressions.data[source_index];
        auto evaluated_expression = evaluator(expression, environment);
        storage.expressions.data[target_index] = evaluated_expression;
    }
    return makeTupleValue(tuple.range, TupleValue{target_indices});
}

template<typename Evaluator, typename Serializer>
static
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Expression table,
    Expression environment
) {
    auto table_struct = storage.table_expressions.data[table.index];
    // Allocation:
    auto rows = std::map<std::string, Row>{};
    FOR_EACH(i, table_struct.rows) {
        auto row = storage.rows.data[i];
        auto key = evaluator(row.key, environment);
        auto value = evaluator(row.value, environment);
        auto serialized_key = StringBuilder{};
        serialized_key = serializer(serialized_key, key);
        auto result = makeStdString(serialized_key);
        FREE_DARRAY(serialized_key);
        rows[result] = {key, value};
    }
    auto code = table.range;
    return makeTableValue(code, TableValue{rows});
}

template<typename Evaluator>
static
Expression evaluateLookupChild(
    Evaluator evaluator, Expression lookup_child, Expression environment
) {
    const auto lookup_child_struct = storage.lookup_child_expressions.data[lookup_child.index];
    const auto child = evaluator(lookup_child_struct.child, environment);
    if (child.type == ERROR_VALUE) {
        return child;
    }
    if (child.type != DICTIONARY_VALUE) {
        auto name = storage.names.data + lookup_child_struct.name;
        return makeErrorValue(lookup_child.range,
            "\n\nI have found an error.\n"
            "It happens when trying to lookup the child named \"%s\" in a dictionary,\n"
            "but instead of a dictionary I got a %s.\n",
            name,
            getExpressionName(child.type)
        );
    }
    const auto dictionary = storage.dictionary_values.data[child.index];
    return requiredLookup(dictionary, lookup_child_struct.name);
}

// Checks `input` against the type ascription of argument `i` of a function,
// given the function's argument_types range. Only in the type-checking pass.
template<bool CheckTypes, typename Evaluator>
static
Expression checkArgument(
    Evaluator evaluator, Indices argument_types, size_t i, Expression input, Expression environment
) {
    if constexpr (CheckTypes) {
        const auto type_expression = storage.argument_types.data[argument_types.data + i];
        if (type_expression.type == ANY_VALUE) {
            return Expression{};
        }
        const auto type = evaluator(type_expression, environment);
        const auto type_check = checkTypes(input, type, "function call");
        if (!type_check.ok) {
            return type_check.error;
        }
    }
    return Expression{};
}

template<bool CheckTypes, typename Evaluator>
static
Expression applyFunction(
    Evaluator evaluator,
    FunctionValue function_value,
    Expression input
) {
    const auto function_struct = storage.function_expressions.data[function_value.function.index];
    const auto argument_check = checkArgument<CheckTypes>(evaluator, function_struct.argument_types, 0, input, function_value.environment);
    if (argument_check.type == ERROR_VALUE) {
        return argument_check;
    }
    // Allocation:
    const auto slot_values = Indices{storage.slot_values.count, 1};
    APPEND(storage.slot_values, input);
    const auto middle = makeDictionaryValue(input.range,
        DictionaryValue{function_value.environment, slot_values, function_struct.argument_names}
    );
    return evaluator(function_struct.body, middle);
}

template<bool CheckTypes, typename Evaluator>
static
Expression applyFunctionDictionary(
    Evaluator evaluator,
    FunctionValue function_value,
    Expression input
) {
    if (input.type != DICTIONARY_VALUE) {
        return makeErrorValue(function_value.function.range,
            "\n\nI have found a type error.\n"
            "It happens when calling a function that is expecting a dictionary as input.\n"
            "But now it got a %s.\n",
            getExpressionName(input.type)
        );
    }
    auto function_struct = storage.function_dictionary_expressions.data[function_value.function.index];
    auto evaluated_dictionary = storage.dictionary_values.data[input.index];
    auto num_arguments = function_struct.argument_names.count;

    // Allocation:
    const auto slot_values = Indices{storage.slot_values.count, num_arguments};
    for (size_t i = 0; i < num_arguments; ++i) {
        const auto name = storage.slot_names.data[function_struct.argument_names.data + i];
        auto expression = requiredLookup(evaluated_dictionary, name);
        const auto argument_check = checkArgument<CheckTypes>(evaluator, function_struct.argument_types, i, expression, function_value.environment);
        if (argument_check.type == ERROR_VALUE) {
            return argument_check;
        }
        APPEND(storage.slot_values, expression);
    }
    auto middle = makeDictionaryValue(input.range,
        DictionaryValue{function_value.environment, slot_values, function_struct.argument_names}
    );
    return evaluator(function_struct.body, middle);
}

template<bool CheckTypes, typename Evaluator>
static
Expression applyFunctionTuple(
    Evaluator evaluator,
    FunctionValue function_value,
    Expression input
) {
    if (input.type != TUPLE_VALUE) {
        return makeErrorValue(function_value.function.range,
            "\n\nI have found a type error.\n"
            "It happens when trying to call a function that takes a tuple.\n"
            "Instead of a tuple I got a %s.\n",
            getExpressionName(input.type)
        );
    }
    auto tuple = storage.tuple_values.data[input.index];
    auto tuple_count = tuple.indices.count;
    auto function_struct = storage.function_tuple_expressions.data[function_value.function.index];
    size_t num_inputs = function_struct.argument_names.count;

    if (num_inputs != tuple_count) {
        return makeErrorValue({},
            "Wrong number of input to function_struct",
            getExpressionName(input.type)
        );
    }

    // Allocation:
    const auto slot_values = Indices{storage.slot_values.count, num_inputs};
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto expression = storage.expressions.data[tuple.indices.data + i];
        const auto argument_check = checkArgument<CheckTypes>(evaluator, function_struct.argument_types, i, expression, function_value.environment);
        if (argument_check.type == ERROR_VALUE) {
            return argument_check;
        }
        APPEND(storage.slot_values, expression);
    }
    const auto middle = makeDictionaryValue(input.range,
        DictionaryValue{function_value.environment, slot_values, function_struct.argument_names}
    );
    return evaluator(function_struct.body, middle);
}

template<bool CheckTypes, typename Evaluator>
static
Expression applyFunctionValue(
    Evaluator evaluator,
    Expression expression,
    Expression input
) {
    const auto function_value = storage.function_values.data[expression.index];
    switch (function_value.function.type) {
        case FUNCTION_EXPRESSION: return applyFunction<CheckTypes>(evaluator, function_value, input);
        case FUNCTION_DICTIONARY_EXPRESSION: return applyFunctionDictionary<CheckTypes>(evaluator, function_value, input);
        case FUNCTION_TUPLE_EXPRESSION: return applyFunctionTuple<CheckTypes>(evaluator, function_value, input);
        default: return makeErrorValue(expression.range,
            "I found an internal error when calling a function.\n"
            "The function value refers to a %s instead of a function literal.",
            getExpressionName(function_value.function.type)
        );
    }
}

static
Expression evaluateFunction(Expression function, Expression environment) {
    return makeFunctionValue(function.range, FunctionValue{function, environment});
}

static
Expression lookupDictionary(CodeRange range, BoundGlobalName name, Expression expression) {
    if (expression.type != DICTIONARY_VALUE) {
        auto symbol = storage.names.data + name.global_index;
        auto expression_name = getExpressionName(expression.type);
        return makeErrorValue(range,
            "Cannot find symbol %s in environment of type %s.\n%s", symbol, expression_name, describeLocation(range));
    }
    const auto dictionary = storage.dictionary_values.data[expression.index];
    if (name.parent_steps == 0) {
        return storage.slot_values.data[dictionary.slot_values.data + name.dictionary_index];
    }
    if (name.parent_steps > 0) {
        return lookupDictionary(
            range,
            BoundGlobalName{name.global_index, name.parent_steps - 1, name.dictionary_index},
            dictionary.environment
        );
    }
    auto symbol = storage.names.data + name.global_index;
    return makeErrorValue(range, "Cannot find symbol %s.\n%s", symbol, describeLocation(range));
}

static
Expression lookupSymbolInDictionary(Expression symbol, Expression environment) {
    auto name = storage.lookup_symbol_expressions.data[symbol.index].name;
    return lookupDictionary(symbol.range, name, environment);
}
    
static
Expression applyFunctionBuiltIn(
    Expression function, Expression input
) {
    if (input.type == ERROR_VALUE) return input;
    const auto function_struct = storage.function_built_in_values.data[function.index];
    return function_struct.function(input);
}

struct BooleanResult {
    bool value;
    Expression error;
};

static
BooleanResult booleanTypes(Expression expression) {
    auto result = MAKE(BooleanResult);
    switch (expression.type) {
        case ERROR_VALUE: return MAKE(BooleanResult, .error=expression);
        case NUMBER: return result;
        case YES: return result;
        case NO: return result;
        case TABLE_VALUE: return result;
        case STACK_VALUE: return result;
        case EMPTY_STACK: return result;
        case STRING: return result;
        case EMPTY_STRING: return result;
        case ANY_VALUE: return result;
        default:
            return MAKE(BooleanResult, .error=makeErrorValue(expression.range,
                "Static type error.\n"
                "Cannot convert type %s to boolean.",
                getExpressionName(expression.type)
            ));
    }
}
    
static
BooleanResult boolean(Expression expression) {
    const auto type = expression.type;
    const auto index = expression.index;
    switch (type) {
    case ERROR_VALUE: return MAKE(BooleanResult, .error=expression);
    case TABLE_VALUE: return MAKE(BooleanResult, .value=!storage.table_values.at(index).empty());
    case TABLE_VIEW_VALUE: return MAKE(BooleanResult, .value=!storage.table_view_values.data[index].empty());
    case NUMBER: return MAKE(BooleanResult, .value=static_cast<bool>(getNumber(expression)));
    case YES: return MAKE(BooleanResult, .value=true);
    case NO: return MAKE(BooleanResult, .value=false);
    case STACK_VALUE: return MAKE(BooleanResult, .value=true);
    case EMPTY_STACK: return MAKE(BooleanResult, .value=false);
    case STRING: return MAKE(BooleanResult, .value=true);
    case EMPTY_STRING: return MAKE(BooleanResult, .value=false);
    default: return MAKE(BooleanResult, .error=makeErrorValue(expression.range,
        "I found an error while trying to evaluate a boolean expression.\n"
        "I got an unexpected type %s.", getExpressionName(type)));
    }
}

static
Expression applyTupleIndexing(Expression tuple, Expression input) {
    const auto tuple_struct = storage.tuple_values.data[tuple.index];
    if (input.type != NUMBER) {
        return makeErrorValue(tuple.range,
            "\n\nI have found a type error.\n"
            "It happens when indexing a tuple.\n"
            "The index is expected to be a %s,\n"
            "but now it is a %s.\n",
            getExpressionName(NUMBER),
            getExpressionName(input.type)
        );
    }
    const auto number = getNumber(input);
    if (number < 0) {
        return makeErrorValue(tuple.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto i = (size_t)number;
    const auto count = tuple_struct.indices.count;
    if (i >= count) {
        return makeErrorValue(tuple.range,
            "Tuple of size %zu indexed with %zu" , count, i
        );
    }
    return storage.expressions.data[tuple_struct.indices.data + i];
}

static
Expression applyTableIndexingTypes(Expression table) {
    const auto& table_struct = storage.table_values.at(table.index);
    if (table_struct.rows.empty()) {
        return Expression{0, table.range, ANY_VALUE};
    }
    return table_struct.begin()->second.value;
}

static
Expression applyStackIndexingTypes(Expression stack) {
    return storage.stack_values.data[stack.index].top;
}

static
Expression applyStringIndexingTypes(Expression string) {
    return storage.strings.data[string.index].top;
}

static
bool isEqual(Expression left, Expression right);

static
bool isTuplePairwiseEqual(TupleValue left, TupleValue right) {
    if (left.indices.count != right.indices.count) {
        return false;
    }
    FOR_EACH2(left_index, right_index, left.indices, right.indices) {
        auto left_item = storage.expressions.data[left_index];
        auto right_item = storage.expressions.data[right_index];
        if (!isEqual(left_item, right_item)) {
            return false;
        }
    }
    return true;
}

static
bool isStackPairwiseEqual(Expression left, Expression right) {
    while (left.type != EMPTY_STACK && right.type != EMPTY_STACK) {
        CHECK_INTERNAL(left.type == STACK_VALUE,
            "Internal error detected in isStackPairwiseEqual.\n"
            "Expected a stack but got a %s",
            getExpressionName(left.type)
        );
        CHECK_INTERNAL(right.type == STACK_VALUE,
            "Internal error detected in isStackPairwiseEqual. "
            "Expected a stack but got a %s",
            getExpressionName(right.type)
        );
        const auto left_container = storage.stack_values.data[left.index];
        const auto right_container = storage.stack_values.data[right.index];
        if (!isEqual(left_container.top, right_container.top)) {
            return false;
        }
        left = left_container.rest;
        right = right_container.rest;
    }
    return left.type == EMPTY_STACK && right.type == EMPTY_STACK;
}

static
bool isStringPairwiseEqual(Expression left, Expression right) {
    while (left.type != EMPTY_STRING && right.type != EMPTY_STRING) {
        CHECK_INTERNAL(left.type == STRING,
            "Internal error detected in isStringPairwiseEqual. "
            "Expected a stack but got a %s",
            getExpressionName(left.type)
        );
        CHECK_INTERNAL(right.type == STRING,
            "Internal error detected in isStringPairwiseEqual. "
            "Expected a stack but got a %s",
            getExpressionName(right.type)
        );
        const auto left_container = storage.strings.data[left.index];
        const auto right_container = storage.strings.data[right.index];
        if (!isEqual(left_container.top, right_container.top)) {
            return false;
        }
        left = left_container.rest;
        right = right_container.rest;
    }
    return left.type == EMPTY_STRING && right.type == EMPTY_STRING;
}


static
bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left) == getNumber(right);
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left) == getCharacter(right);
    }
    if (left_type == YES && right_type == YES) {
        return true;
    }
    if (left_type == NO && right_type == NO) {
        return true;
    }
    if (left_type == EMPTY_STACK && right_type == EMPTY_STACK) {
        return true;
    }
    if (left_type == STACK_VALUE && right_type == STACK_VALUE) {
        return isStackPairwiseEqual(left, right);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return isStringPairwiseEqual(left, right);
    }
    if (left_type == TUPLE_VALUE && right_type == TUPLE_VALUE) {
        return isTuplePairwiseEqual(
            storage.tuple_values.data[left.index],
            storage.tuple_values.data[right.index]
        );
    }
    return false;
}

static
Expression evaluateDynamicExpressionTyped(Expression expression) {
    return Expression{0, expression.range, ANY_VALUE};
}

static
Expression evaluateDynamicExpression(Expression expression, Expression environment) {
    const auto inner_expression = storage.dynamic_expressions.data[expression.index].expression;
    return evaluate(inner_expression, environment);
}

static
Expression evaluateConditionalTypes(
    Expression conditional, Expression environment
) {
    const auto conditional_struct = storage.conditional_expressions.data[conditional.index];
    FOR_EACH(a, conditional_struct.alternatives) {
        auto result = evaluate_types(storage.alternatives.data[a].left, environment);
        if (result.type == ERROR_VALUE) return result;
    }
    const auto else_expression = evaluate_types(conditional_struct.expression_else, environment);
    if (else_expression.type == ERROR_VALUE) return else_expression;
    FOR_EACH(a, conditional_struct.alternatives) {
        const auto alternative = storage.alternatives.data[a];
        const auto alternative_expression = evaluate_types(
            alternative.right, environment
        );
        auto type_check = checkTypes(else_expression, alternative_expression, "if");
        if (!type_check.ok) return type_check.error;
    }
    return else_expression;
}

static
Expression evaluateConditional(Expression conditional, Expression environment) {
    const auto conditional_struct = storage.conditional_expressions.data[conditional.index];
    FOR_EACH(a, conditional_struct.alternatives) {
        const auto alternative = storage.alternatives.data[a];
        const auto condition = boolean(evaluate(alternative.left, environment));
        if (condition.error.type == ERROR_VALUE) {
            return condition.error;
        }
        if (condition.value) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(conditional_struct.expression_else, environment);
}

static
Expression evaluateIsTypes(
    Expression is, Expression environment
) {
    const auto is_struct = storage.is_expressions.data[is.index];
    auto result = evaluate_types(is_struct.input, environment);
    if (result.type == ERROR_VALUE) return result;
    FOR_EACH(a, is_struct.alternative) {
        const auto alternative = storage.alternatives.data[a];
        result = evaluate_types(alternative.left, environment);
        if (result.type == ERROR_VALUE) return result;
    }
    const auto else_expression = evaluate_types(is_struct.expression_else, environment);
    FOR_EACH(a, is_struct.alternative) {
        const auto alternative = storage.alternatives.data[a];
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        if (alternative_expression.type == ERROR_VALUE) return alternative_expression;
        auto type_check = checkTypes(else_expression, alternative_expression, "is");
        if (!type_check.ok) return type_check.error;
    }
    return else_expression;
}

static
Expression evaluateIs(Expression is, Expression environment) {
    const auto is_struct = storage.is_expressions.data[is.index];
    const auto value = evaluate(is_struct.input, environment);
    FOR_EACH(a, is_struct.alternative) {
        const auto alternative = storage.alternatives.data[a];
        const auto left_value = evaluate(alternative.left, environment);
        if (isEqual(value, left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(is_struct.expression_else, environment);
}

static
Expression evaluateTypedExpressionTypes(Expression expression, Expression environment) {
    const auto type = evaluate_types(storage.typed_expressions.data[expression.index].type, environment);
    const auto value = evaluate_types(storage.typed_expressions.data[expression.index].value, environment);
    const auto type_check = checkTypes(type, value, "typed expression");
    if (!type_check.ok) {
        return type_check.error;
    }
    return value;
}
    
static
Expression evaluateTypedExpression(Expression expression, Expression environment) {
    return evaluate(storage.typed_expressions.data[expression.index].value, environment);
}

// Allocates the slots of a new dictionary value, all holding the any-value.
// A slot defined by a statement gets the range of that statement, so that a
// read before the definition can be reported at the definition.
static
Indices initializeDefinitions(const DictionaryExpression& dictionary) {
    // Allocation:
    const auto first = storage.slot_values.count;
    const auto slot_values = Indices{first, dictionary.slot_count};
    for (size_t i = 0; i < dictionary.slot_count; ++i) {
        APPEND(storage.slot_values, Expression{});
    }
    FOR_EACH(i, dictionary.statements) {
        auto statement = storage.statements.data[i];
        auto type = statement.type;
        if (type == DEFINITION_STATEMENT) {
            auto dictionary_index = storage.definition_statements.data[statement.index].name.dictionary_index;
            storage.slot_values.data[first + dictionary_index] = Expression{0, statement.range, ANY_VALUE};
        }
        else if (type == FOR_INIT_STATEMENT) {
            auto dictionary_index = storage.for_init_statements.data[statement.index].name.dictionary_index;
            storage.slot_values.data[first + dictionary_index] = Expression{0, statement.range, ANY_VALUE};
        }
    }
    return slot_values;
}

static
void setSlot(Expression evaluated_dictionary, size_t slot_index, Expression value) {
    CHECK_INTERNAL(
        evaluated_dictionary.type == DICTIONARY_VALUE,
        "setSlot expected %s got %s",
        getExpressionName(DICTIONARY_VALUE),
        getExpressionName(evaluated_dictionary.type)
    );
    auto first = storage.dictionary_values.data[evaluated_dictionary.index].slot_values.data;
    storage.slot_values.data[first + slot_index] = value;
}

static
Expression getSlot(Expression evaluated_dictionary, size_t slot_index) {
    CHECK_INTERNAL(
        evaluated_dictionary.type == DICTIONARY_VALUE,
        "getSlot expected %s got %s",
        getExpressionName(DICTIONARY_VALUE),
        getExpressionName(evaluated_dictionary.type)
    );
    auto first = storage.dictionary_values.data[evaluated_dictionary.index].slot_values.data;
    return storage.slot_values.data[first + slot_index];
}

static
void setDictionaryDefinition(
    Expression evaluated_dictionary, BoundLocalName name, Expression value
) {
    setSlot(evaluated_dictionary, name.dictionary_index, value);
}

static
Expression getDictionaryDefinition(
    Expression evaluated_dictionary, BoundLocalName name
) {
    return getSlot(evaluated_dictionary, name.dictionary_index);
}

static
Expression evaluateDictionaryTypes(
    Expression dictionary, Expression environment
) {
    const auto initial_definitions = initializeDefinitions(
        storage.dictionary_expressions.data[dictionary.index]
    );
    const auto result = makeDictionaryValue(
        dictionary.range, DictionaryValue{
            environment,
            initial_definitions,
            storage.dictionary_expressions.data[dictionary.index].names
        }
    );
    const auto dictionary_struct = storage.dictionary_expressions.data[dictionary.index];
    FOR_EACH(i, dictionary_struct.statements) {
        const auto statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == DEFINITION_STATEMENT) {
            const auto definition = storage.definition_statements.data[statement.index];
            const auto right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result);
            if (value.type == ERROR_VALUE) return value;
            // TODO: is this a principled approach?
            if (value.type != ANY_VALUE) {
                setDictionaryDefinition(result, definition.name, value);
            }
        }
        else if (type == PUT_ASSIGNMENT_STATEMENT) {
            const auto put_assignment = storage.put_assignment_statements.data[statement.index];
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result);
            if (value.type == ERROR_VALUE) return value;
            const auto current = getDictionaryDefinition(result, put_assignment.name);
            const auto tuple = makeTupleValue2(value, current);
            const auto new_value = builtInPutTyped(tuple);
            setDictionaryDefinition(result, put_assignment.name, new_value);
        }
        else if (type == PUT_EACH_ASSIGNMENT_STATEMENT) {
            const auto put_each_assignment = storage.put_each_assignment_statements.data[statement.index];
            const auto right_expression = put_each_assignment.expression;
            auto container = evaluate_types(right_expression, result);
            if (container.type == ERROR_VALUE) {
                return container;
            }

            {
                const auto current = getDictionaryDefinition(result, put_each_assignment.name);
                const auto value = builtInTakeTyped(container);
                const auto tuple = makeTupleValue2(value, current);
                const auto new_value = builtInPutTyped(tuple);
                setDictionaryDefinition(result, put_each_assignment.name, new_value);
            }
        }
        else if (type == DROP_ASSIGNMENT_STATEMENT) {
            const auto drop_assignment = storage.drop_assignment_statements.data[statement.index];
            const auto current = getDictionaryDefinition(result, drop_assignment.name);
            const auto new_value = builtInDropTyped(current);
            setDictionaryDefinition(result, drop_assignment.name, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.data[statement.index];
            auto condition = booleanTypes(evaluate_types(while_statement.expression, result));
            if (condition.error.type == ERROR_VALUE) return condition.error;
        }
        else if (type == FOR_INIT_STATEMENT) {
            const auto for_init_statement = storage.for_init_statements.data[statement.index];
            const auto container = evaluate_types(for_init_statement.container_expression, result);
            if (container.type == ERROR_VALUE) return container;
            auto condition = booleanTypes(container);
            if (condition.error.type == ERROR_VALUE) return condition.error;
            const auto value = builtInTakeTyped(container);
            setDictionaryDefinition(result, for_init_statement.name, value);
        }
        else if (type == IF_STATEMENT) {
            const auto if_statement = storage.if_statements.data[statement.index];
            auto condition = booleanTypes(evaluate_types(if_statement.expression, result));
            if (condition.error.type == ERROR_VALUE) return condition.error;
        }
        else if (type == RETURN_STATEMENT) {
        }
    }
    return result;
}

static
Expression evaluateDictionary(Expression dictionary, Expression environment) {
    const auto initial_definitions = initializeDefinitions(
        storage.dictionary_expressions.data[dictionary.index]
    );
    const auto result = makeDictionaryValue(
        dictionary.range, DictionaryValue{
            environment,
            initial_definitions,
            storage.dictionary_expressions.data[dictionary.index].names
        }
    );

    const auto dict_statements = storage.dictionary_expressions.data[dictionary.index].statements;
    const auto base_index = dict_statements.data;
    const auto statement_count = dict_statements.count;
    
    auto i = size_t{0};
    while (i < statement_count) {
        const auto statement = storage.statements.data[base_index + i];
        const auto type = statement.type;
        if (type == DEFINITION_STATEMENT) {
            const auto definition = storage.definition_statements.data[statement.index];
            const auto right_expression = definition.expression;
            const auto value = evaluate(right_expression, result);
            // Bottleneck:
            setDictionaryDefinition(result, definition.name, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT_STATEMENT) {
            const auto put_assignment = storage.put_assignment_statements.data[statement.index];
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate(right_expression, result);
            const auto current = getDictionaryDefinition(result, put_assignment.name);
            const auto tuple = makeTupleValue2(value, current);
            const auto new_value = builtInPut(tuple);
            setDictionaryDefinition(result, put_assignment.name, new_value);
            i += 1;
        }
        else if (type == PUT_EACH_ASSIGNMENT_STATEMENT) {
            const auto put_each_assignment = storage.put_each_assignment_statements.data[statement.index];
            const auto right_expression = put_each_assignment.expression;
            auto container = evaluate(right_expression, result);
            for (;;) {
                auto condition = boolean(container);
                if (condition.error.type == ERROR_VALUE) {
                    return condition.error;
                }
                if (!condition.value) {
                    break;
                }
                const auto current = getDictionaryDefinition(result, put_each_assignment.name);
                const auto value = builtInTake(container);
                const auto tuple = makeTupleValue2(value, current);
                const auto new_value = builtInPut(tuple);
                setDictionaryDefinition(result, put_each_assignment.name, new_value);
                container = builtInDrop(container);
            }
            i += 1;
        }
        else if (type == DROP_ASSIGNMENT_STATEMENT) {
            const auto drop_assignment = storage.drop_assignment_statements.data[statement.index];
            const auto current = getDictionaryDefinition(result, drop_assignment.name);
            const auto new_value = builtInDrop(current);
            setDictionaryDefinition(result, drop_assignment.name, new_value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.data[statement.index];
            auto condition = boolean(evaluate(while_statement.expression, result));
            if (condition.error.type == ERROR_VALUE) {
                return condition.error;
            }
            if (condition.value) {
                i += 1;
            } else {
                i = while_statement.end_index + 1;
            }
        }
        else if (type == FOR_INIT_STATEMENT) {
            const auto for_init_statement = storage.for_init_statements.data[statement.index];
            const auto container = evaluate(for_init_statement.container_expression, result);
            auto condition = boolean(container);
            if (condition.error.type == ERROR_VALUE) {
                return condition.error;
            }
            if (condition.value) {
                setSlot(result, for_init_statement.container_index, container);
                setDictionaryDefinition(result, for_init_statement.name, builtInTake(container));
                i += 1;
            } else {
                auto for_statement = storage.statements.data[base_index + i + 1];
                i = storage.for_statements.data[for_statement.index].end_index + 1;
            }
        }
        else if (type == FOR_STATEMENT) {
            i += 1;
        }
        else if (type == IF_STATEMENT) {
            const auto if_statement = storage.if_statements.data[statement.index];
            auto condition = boolean(evaluate(if_statement.expression, result));
            if (condition.error.type == ERROR_VALUE) {
                return condition.error;
            }
            if (condition.value) {
                i += 1;
            } else {
                i = if_statement.end_index + 1;
            }
        }
        else if (type == WHILE_END_STATEMENT) {
            const auto end_statement = storage.while_end_statements.data[statement.index];
            i = end_statement.start_index;
        }
        else if (type == FOR_END_STATEMENT) {
            auto end_statement = storage.for_end_statements.data[statement.index];
            auto start_statement = storage.statements.data[base_index + end_statement.start_index];
            auto for_statement = storage.for_statements.data[start_statement.index];
            auto next_container = builtInDrop(getSlot(result, for_statement.container_index));
            auto condition = boolean(next_container);
            if (condition.error.type == ERROR_VALUE) {
                return condition.error;
            }
            if (condition.value) {
                setSlot(result, for_statement.container_index, next_container);
                setDictionaryDefinition(result, for_statement.name, builtInTake(next_container));
                i = end_statement.start_index;
            } else {
                // The named slot keeps the last item.
                i += 1;
            }
        }
        else if (type == IF_END_STATEMENT) {
            i += 1;
        }
        else if (type == RETURN_STATEMENT) {
            break;
        }
    }
    return result;
}

static
std::string stdStringFromManglang(Expression key) {
    auto buffer = StringBuilder{};
    buffer = serialize(buffer, key);
    auto k = makeStdString(buffer);
    FREE_DARRAY(buffer);
    return k;
}
    
static
Expression applyTableIndexing(Expression table, Expression key) {
    const auto& table_struct = storage.table_values.at(table.index);
    const auto& rows = table_struct.rows;
    auto k = stdStringFromManglang(key);
    auto it = rows.find(k);
    if (it == rows.end()) {
        return makeErrorValue(table.range, "Cannot find key %s in table", k.c_str());
    }
    return it->second.value;
}

static
Expression applyStackIndexing(Expression stack, Expression input) {
    if (input.type != NUMBER) {
        return makeErrorValue(stack.range,
            "\n\nI have found a dynamic type error.\n"
            "It happens when indexing a stack.\n"
            "The index is expected to be a %s,\n"
            "but now it is a %s.\n",
            getExpressionName(NUMBER),
            getExpressionName(input.type)
        );
    }
    const auto number = getNumber(input);
    if (number < 0) {
        return makeErrorValue(stack.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto index = (size_t)number;
    auto stack_struct = storage.stack_values.data[stack.index];
    for (size_t i = 0; i < index; ++i) {
        if (stack_struct.rest.type == EMPTY_STACK) {
            return makeErrorValue(stack.range,
                "Stack index out of range"
            );
        }
        if (stack_struct.rest.type != STACK_VALUE) {
            return makeErrorValue(stack.range,
                "I found a type error while indexing a stack. \n"
                "Instead of a stack I encountered a %s",
                getExpressionName(stack_struct.rest.type)
            );
        }
        stack_struct = storage.stack_values.data[stack_struct.rest.index];
    }
    return stack_struct.top;
}

static
Expression applyStringIndexing(Expression string, Expression input) {
    if (input.type != NUMBER) {
        return makeErrorValue(string.range,
            "\n\nI have found a dynamic type error.\n"
            "It happens when indexing a string.\n"
            "The index is expected to be a %s,\n"
            "but now it is a %s.\n",
            getExpressionName(NUMBER),
            getExpressionName(input.type)
        );
    }
    const auto number = getNumber(input);
    if (number < 0) {
        return makeErrorValue(string.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto index = (size_t)number;
    auto string_struct = storage.strings.data[string.index];
    for (size_t i = 0; i < index; ++i) {
        if (string_struct.rest.type == EMPTY_STACK) {
            return makeErrorValue(string.range,
                "String index out of range"
            );
        }
        if (string_struct.rest.type != STRING) {
            return makeErrorValue(string.range,
                "I found a type error while indexing a string. \n"
                "Instead of a string I encountered a %s",
                getExpressionName(string_struct.rest.type)
            );
        }
        string_struct = storage.strings.data[string_struct.rest.index];
    }
    return string_struct.top;
}

static
Expression evaluateFunctionApplicationTypes(
    Expression function_application, Expression environment
) {
    auto name = storage.function_application_expressions.data[function_application.index].name;
    const auto function = lookupDictionary(function_application.range, name, environment);
    const auto input = evaluate_types(
        storage.function_application_expressions.data[function_application.index].child,
        environment
    );
    if (input.type == ERROR_VALUE) return input;
    switch (function.type) {
        case ERROR_VALUE: return function;

        case FUNCTION_VALUE: return applyFunctionValue<true>(evaluate_types, function, input);
        case FUNCTION_BUILT_IN_VALUE: return applyFunctionBuiltIn(function, input);

        case TABLE_VALUE: return applyTableIndexingTypes(function);
        case TUPLE_VALUE: return applyTupleIndexing(function, input);
        case STACK_VALUE: return applyStackIndexingTypes(function);
        case STRING: return applyStringIndexingTypes(function);

        case EMPTY_STACK: return Expression{0, function_application.range, ANY_VALUE};
        case EMPTY_STRING: return Expression{0, function_application.range, CHARACTER};
    
        default: return makeErrorValue(function_application.range,
            "I found an error during type checking.\n"
            "The application operator (!) received an %s, which I did not expect.",
            getExpressionName(function.type)
        );
    }
}

static
Expression evaluateFunctionApplication(
    Expression function_application, Expression environment
) {
    auto name = storage.function_application_expressions.data[function_application.index].name;
    const auto function = lookupDictionary(function_application.range, name, environment);
    const auto input = evaluate(
        storage.function_application_expressions.data[function_application.index].child,
        environment
    );
    switch (function.type) {
        case ERROR_VALUE: return function;

        case FUNCTION_VALUE: return applyFunctionValue<false>(evaluate, function, input);
        case FUNCTION_BUILT_IN_VALUE: return applyFunctionBuiltIn(function, input);

        case TABLE_VALUE: return applyTableIndexing(function, input);
        case TUPLE_VALUE: return applyTupleIndexing(function, input);
        case STACK_VALUE: return applyStackIndexing(function, input);
        case STRING: return applyStringIndexing(function, input);
        
        case EMPTY_STACK: return makeErrorValue(function_application.range,
            "I caught a run-time error when trying to index an empty stack.");
        case EMPTY_STRING: return makeErrorValue(function_application.range,
            "I caught a run-time error when trying to index an empty string.");

        default: return makeErrorValue(function_application.range,
            "I found an error during evaluation.\n"
            "The application operator (!) received an %s, which I did not expect.",
            getExpressionName(function.type)
        );
    }
}

static
Expression evaluateFunctionApplicationBuiltInTypes(Expression built_in_application, Expression environment) {
    auto built_in = storage.function_application_built_in_expressions.data[built_in_application.index];
    auto input = evaluate_types(built_in.child, environment);
    if (input.type == ERROR_VALUE) return input;
    return built_in.function_types(input);
}

static
Expression evaluateFunctionApplicationBuiltIn(Expression built_in_application, Expression environment) {
    auto built_in = storage.function_application_built_in_expressions.data[built_in_application.index];
    auto input = evaluate(built_in.child, environment);
    if (input.type == ERROR_VALUE) return input;
    return built_in.function(input);
}

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case ERROR_VALUE: return expression;
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case STACK_VALUE: return expression;
        case DICTIONARY_VALUE: return expression;
        case TUPLE_VALUE: return expression;
        case TABLE_VALUE: return expression;
        case TABLE_VIEW_VALUE: return expression;
        case FUNCTION_VALUE: return expression;
        case FUNCTION_BUILT_IN_VALUE: return expression;

        // These are the same for types and values:
        case FUNCTION_EXPRESSION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE_EXPRESSION: return evaluateFunction(expression, environment);
        case FUNCTION_DICTIONARY_EXPRESSION: return evaluateFunction(expression, environment);
        case LOOKUP_SYMBOL_EXPRESSION: return lookupSymbolInDictionary(expression, environment);

        // These are different for types and values, but templated:
        case STACK_EXPRESSION: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE_EXPRESSION: return evaluateTuple(evaluate_types, expression, environment);
        case TABLE_EXPRESSION: return evaluateTable(evaluate_types, serialize_types, expression, environment);
        case LOOKUP_CHILD_EXPRESSION: return evaluateLookupChild(evaluate_types, expression, environment);

        // These are different for types and values:
        case TYPED_EXPRESSION: return evaluateTypedExpressionTypes(expression, environment);
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpressionTyped(expression);
        case CONDITIONAL_EXPRESSION: return evaluateConditionalTypes(expression, environment);
        case IS_EXPRESSION: return evaluateIsTypes(expression, environment);
        case DICTIONARY_EXPRESSION: return evaluateDictionaryTypes(expression, environment);
        case FUNCTION_APPLICATION_EXPRESSION: return evaluateFunctionApplicationTypes(expression, environment);
        case FUNCTION_APPLICATION_BUILT_IN_EXPRESSION: return evaluateFunctionApplicationBuiltInTypes(expression, environment);

        default: return makeErrorValue(expression.range,
            "I found an error during type checking.\n"
            "I received an %s, which I did not expect.",
            getExpressionName(expression.type)
        );
    }
}

Expression evaluate(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case ERROR_VALUE: return expression;
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case STACK_VALUE: return expression;
        case DICTIONARY_VALUE: return expression;
        case TUPLE_VALUE: return expression;
        case TABLE_VALUE: return expression;
        case TABLE_VIEW_VALUE: return expression;
        case FUNCTION_VALUE: return expression;
        case FUNCTION_BUILT_IN_VALUE: return expression;

        // These are the same for types and values:
        case FUNCTION_EXPRESSION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE_EXPRESSION: return evaluateFunction(expression, environment);
        case FUNCTION_DICTIONARY_EXPRESSION: return evaluateFunction(expression, environment);
        case LOOKUP_SYMBOL_EXPRESSION: return lookupSymbolInDictionary(expression, environment);

        // These are different for types and values, but templated:
        case STACK_EXPRESSION: return evaluateStack(evaluate, expression, environment);
        case TUPLE_EXPRESSION: return evaluateTuple(evaluate, expression, environment);
        case TABLE_EXPRESSION: return evaluateTable(evaluate, serialize, expression, environment);
        case LOOKUP_CHILD_EXPRESSION: return evaluateLookupChild(evaluate, expression, environment);

        // These are different for types and values:
        case TYPED_EXPRESSION: return evaluateTypedExpression(expression, environment);
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpression(expression, environment);
        case CONDITIONAL_EXPRESSION: return evaluateConditional(expression, environment);
        case IS_EXPRESSION: return evaluateIs(expression, environment);
        case DICTIONARY_EXPRESSION: return evaluateDictionary(expression, environment);
        case FUNCTION_APPLICATION_EXPRESSION: return evaluateFunctionApplication(expression, environment);
        case FUNCTION_APPLICATION_BUILT_IN_EXPRESSION: return evaluateFunctionApplicationBuiltIn(expression, environment);

        default: return makeErrorValue(expression.range,
            "I found an error during evaluation.\n"
            "I received an %s, which I did not expect.",
            getExpressionName(expression.type)
        );
    }
}
