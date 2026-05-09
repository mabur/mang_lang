#include "evaluate.h"

#include <cassert>
#include <string.h>

#include <carma/carma.h>

#include "../built_in_functions/container.h"
#include "../exceptions.h"
#include "../factory.h"
#include "../string.h"
#include "../type_check.h"
#include "serialize.h"

namespace {

struct OptionalLookup {
    Expression value;
    bool ok;
};
    
OptionalLookup optionalLookup(EvaluatedDictionary dictionary, size_t name) {
    auto result = MAKE(OptionalLookup);
    FOR_EACH(i, dictionary.definitions) {
        if (storage.definitions.data[i].name.global_index == name) {
            result.value = storage.definitions.data[i].expression;
            result.ok = true;
        }
    }
    return result;
}

Expression requiredLookup(EvaluatedDictionary dictionary, size_t name) {
    const auto result = optionalLookup(dictionary, name);
    if (result.ok) {
        return result.value;
    }
    auto name_c = storage.names.data + name;
    return makeEvaluateError({}, "I cannot find name %s in dictionary", name_c);
}
    
TypeCheck checkTypes(Expression super, Expression sub, const char* description);

TypeCheck checkTypesEvaluatedStack(Expression super, Expression sub, const char* description) {
    const auto stack_super = storage.evaluated_stacks.data[super.index].top;
    const auto stack_sub = storage.evaluated_stacks.data[sub.index].top;
    return checkTypes(stack_super, stack_sub, description);
}

TypeCheck checkTypesEvaluatedTable(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto& table_super = storage.evaluated_tables.at(super.index);
    const auto& table_sub = storage.evaluated_tables.at(sub.index);
    if (table_super.empty()) return result;
    if (table_sub.empty()) return result;
    const auto row_super = table_super.begin()->second;
    const auto row_sub = table_sub.begin()->second;
    result = checkTypes(row_super.key, row_sub.key, description);
    if (!result.ok) return result;
    result = checkTypes(row_super.value, row_sub.value, description);
    return result;
}

TypeCheck checkTypesEvaluatedTuple(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto& tuple_super = storage.evaluated_tuples.data[super.index];
    const auto& tuple_sub = storage.evaluated_tuples.data[sub.index];
    const auto super_count = tuple_super.indices.count;
    const auto sub_count = tuple_sub.indices.count;
    if (super_count != sub_count) {
        result.ok = false;
        result.error = makeEvaluateError({},
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

TypeCheck checkTypesEvaluatedDictionary(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    const auto& dictionary_super = storage.evaluated_dictionaries.data[super.index];
    const auto& dictionary_sub = storage.evaluated_dictionaries.data[sub.index];
    FOR_EACH(i, dictionary_super.definitions) {
        auto definition_super = storage.definitions.data[i];
        const auto name_super = definition_super.name.global_index;
        const auto value_sub = optionalLookup(dictionary_sub, name_super);
        if (value_sub.ok) {
            result = checkTypes(definition_super.expression, value_sub.value, description);
            if (!result.ok) return result;
        }
        else {
            result.ok = false;
            result.error = makeEvaluateError({},
                "Static type error in %s. Could not find name %s in dictionary %s",
                description,
                storage.names.data + name_super,
                describeLocation(sub.range)
            );
            return result;
        }
    }
    return result;
}

TypeCheck checkTypes(Expression super, Expression sub, const char* description) {
    auto result = TypeCheck{.ok=true};
    if (super.type == ANY || sub.type == ANY) return result;
    
    if (super.type == NUMBER && sub.type == NUMBER) return result;
    if (super.type == CHARACTER && sub.type == CHARACTER) return result;
    
    if (super.type == NO && sub.type == NO) return result;
    if (super.type == YES && sub.type == YES) return result;
    if (super.type == YES && sub.type == NO) return result;
    if (super.type == NO && sub.type == YES) return result;

    if (super.type == FUNCTION && sub.type == FUNCTION) return result;
    if (super.type == FUNCTION && sub.type == FUNCTION_DICTIONARY) return result;
    if (super.type == FUNCTION && sub.type == FUNCTION_TUPLE) return result;
    if (super.type == FUNCTION && sub.type == FUNCTION_BUILT_IN) return result;
    if (super.type == FUNCTION_DICTIONARY && sub.type == FUNCTION) return result;
    if (super.type == FUNCTION_TUPLE && sub.type == FUNCTION) return result;
    if (super.type == FUNCTION_BUILT_IN && sub.type == FUNCTION) return result;
    
    if (super.type == EMPTY_STRING && sub.type == EMPTY_STRING) return result;
    if (super.type == EMPTY_STRING && sub.type == STRING) return result;
    if (super.type == STRING && sub.type == EMPTY_STRING) return result;
    if (super.type == STRING && sub.type == STRING) return result;
    
    if (super.type == EMPTY_STACK && sub.type == EMPTY_STACK) return result;
    if (super.type == EMPTY_STACK && sub.type == EVALUATED_STACK) return result;
    if (super.type == EVALUATED_STACK && sub.type == EMPTY_STACK) return result;
    if (super.type == EVALUATED_STACK && sub.type == EVALUATED_STACK) {
        return checkTypesEvaluatedStack(super, sub, description);
    }
    if (super.type == EVALUATED_TABLE && sub.type == EVALUATED_TABLE) {
        return checkTypesEvaluatedTable(super, sub, description);
    }
    if (super.type == EVALUATED_TUPLE && sub.type == EVALUATED_TUPLE) {
        return checkTypesEvaluatedTuple(super, sub, description);
    }
    if (super.type == EVALUATED_DICTIONARY && sub.type == EVALUATED_DICTIONARY) {
        return checkTypesEvaluatedDictionary(super, sub, description);
    }
    result.ok = false;
    result.error = makeEvaluateError({},
        "Static type error in %s at %s. %s is not a supertype for %s",
        description,
        describeLocation(super.range),
        getExpressionName(super.type),
        getExpressionName(sub.type)
    );
    return result;
}

template<typename Evaluator>
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    // Allocation:
    auto items = Expressions{};
    while (stack.type != EMPTY_STACK) {
        if (stack.type != STACK) {
            return makeEvaluateError(stack.range,
                "\n\nI have found a type error.\n"
                "It happens in evaluateStack.\n"
                "Instead of a stack I got a %s.\n",
                getExpressionName(stack.type));
        }
        const auto stack_struct = storage.stacks.data[stack.index];
        const auto& top = stack_struct.top;
        const auto& rest = stack_struct.rest;
        APPEND(items, evaluator(top, environment));
        stack = rest;
    }
    auto evaluated_stack = Expression{0, stack.range, EMPTY_STACK};
    FOR_EACH_BACKWARD(it, items) {
        evaluated_stack = putEvaluatedStack(evaluated_stack, *it);
    }
    FREE_DARRAY(items);
    return evaluated_stack;
}

template<typename Evaluator>
Expression evaluateTuple(
    Evaluator evaluator, Expression tuple, Expression environment
) {
    auto tuple_struct = storage.tuples.data[tuple.index];
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
    return makeEvaluatedTuple(tuple.range, EvaluatedTuple{target_indices});
}

template<typename Evaluator, typename Serializer>
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Expression table,
    Expression environment
) {
    auto table_struct = storage.tables.data[table.index];
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
    return makeEvaluatedTable(code, EvaluatedTable{rows});
}

template<typename Evaluator>
Expression evaluateLookupChild(
    Evaluator evaluator, Expression lookup_child, Expression environment
) {
    const auto lookup_child_struct = storage.child_lookups.data[lookup_child.index];
    const auto child = evaluator(lookup_child_struct.child, environment);
    if (child.type != EVALUATED_DICTIONARY) {
        return makeEvaluateError({},
            "\n\nI have found a type error.\n"
            "It happens when trying to lookup a child in a dictionary,\n"
            "but instead of a dictionary I got a %s.\n",
            getExpressionName(child.type)
        );
    }
    const auto& dictionary = storage.evaluated_dictionaries.data[child.index];
    return requiredLookup(dictionary, lookup_child_struct.name);
}

template<typename Evaluator>
void checkArgument(
    Evaluator evaluator, const Argument& a, Expression input, Expression environment
) {
    if (a.type.type == ANY) {
        return;
    }
    const auto type = evaluator(a.type, environment);
    checkTypes(input, type, "function call");
}

template<typename Evaluator>
Expression applyFunction(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    const auto function_struct = storage.functions.data[function.index];
    const auto argument = storage.arguments.data[function_struct.argument];
    checkArgument(evaluator, argument, input, function_struct.environment);
    // TODO: allocate on storage.definitions directly?
    // This is a trade-off between heap fragmentation and automated memory cleanup.
    // Allocation:
    auto first = storage.definitions.count;
    makeDefinition({}, Definition{BoundLocalName{argument.name, 0}, input});
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    const auto middle = makeEvaluatedDictionary(input.range,
        EvaluatedDictionary{function_struct.environment, definitions}
    );
    return evaluator(function_struct.body, middle);
}

template<typename Evaluator>
Expression applyFunctionDictionary(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    if (input.type != EVALUATED_DICTIONARY) {
        return makeEvaluateError(function.range,
            "\n\nI have found a type error.\n"
            "It happens when calling a function that is expecting a dictionary as input.\n"
            "But now it got a %s.\n",
            getExpressionName(input.type)
        );
    }
    const auto function_struct = storage.dictionary_functions.data[function.index];
    const auto& evaluated_dictionary = storage.evaluated_dictionaries.data[input.index];
    FOR_EACH(i, function_struct.arguments) {
        const auto argument = storage.arguments.data[i];
        const auto expression = requiredLookup(evaluated_dictionary, argument.name);
        checkArgument(evaluator, argument, expression, function_struct.environment);
    }
    // TODO: pass along environment? Is some use case missing now?
    return evaluator(function_struct.body, input);
}

template<typename Evaluator>
Expression applyFunctionTuple(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    if (input.type != EVALUATED_TUPLE) {
        return makeEvaluateError(function.range,
            "\n\nI have found a type error.\n"
            "It happens when trying to call a function that takes a tuple.\n"
            "Instead of a tuple I got a %s.\n",
            getExpressionName(input.type)
        );
    }
    const auto tuple = storage.evaluated_tuples.data[input.index];
    const auto tuple_count = tuple.indices.count;
    const auto function_struct = storage.tuple_functions.data[function.index];
    const auto first_argument = BEGIN_POINTER(function_struct.arguments);
    const auto last_argument = END_POINTER(function_struct.arguments);
    const auto num_inputs = last_argument - first_argument;
    
    if (num_inputs != tuple_count) {
        return makeEvaluateError({},
            "Wrong number of input to function_struct",
            getExpressionName(input.type)
        );
    }

    auto argument_index = first_argument;
    // TODO: allocate on storage.definitions directly.
    // This is a trade-off between heap fragmentation and automated memory cleanup.
    // Allocation:
    auto first = storage.definitions.count;
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto argument = storage.arguments.data[argument_index + i];
        const auto expression = storage.expressions.data[tuple.indices.data + i];
        checkArgument(evaluator, argument, expression, function_struct.environment);
        makeDefinition({}, Definition{BoundLocalName{argument.name, i}, expression});
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    const auto middle = makeEvaluatedDictionary(input.range,
        EvaluatedDictionary{function_struct.environment, definitions}
    );
    return evaluator(function_struct.body, middle);
}

Expression evaluateFunction(Expression function, Expression environment) {
    const auto function_struct = storage.functions.data[function.index];
    return makeFunction(function.range, {
        environment, function_struct.argument, function_struct.body
    });
}

Expression evaluateFunctionDictionary(
    Expression function_dictionary, Expression environment
) {
    const auto function_dictionary_struct = storage.dictionary_functions.data[function_dictionary.index];
    return makeFunctionDictionary(function_dictionary.range, {
        environment,
        function_dictionary_struct.arguments,
        function_dictionary_struct.body
    });
}

Expression evaluateFunctionTuple(
    Expression function_tuple, Expression environment
) {
    const auto function_tuple_struct = storage.tuple_functions.data[function_tuple.index];
    return makeFunctionTuple(function_tuple.range, {
        environment, function_tuple_struct.arguments, function_tuple_struct.body
    });
}

struct LookupResult {
    Expression expression;
    size_t steps;
};

LookupResult lookupDictionaryFirstTime(
    const BoundGlobalName& name, size_t steps, Expression expression
) {
    if (expression.type != EVALUATED_DICTIONARY) {
        auto symbol = storage.names.data + name.global_index;
        return LookupResult{makeEvaluateError({},
            "Cannot find symbol %s in environment of type %s", symbol, getExpressionName(expression.type))};
    }
    const auto& dictionary = storage.evaluated_dictionaries.data[expression.index];
    const auto result = optionalLookup(dictionary, name.global_index);
    if (result.ok) {
        return LookupResult{result.value, steps};
    }
    return lookupDictionaryFirstTime(name, steps + 1, dictionary.environment);
}

Expression lookupDictionarySecondTime(
    const BoundGlobalName& name, int steps, Expression expression
) {
    if (expression.type != EVALUATED_DICTIONARY) {
        auto symbol = storage.names.data + name.global_index;
        return makeEvaluateError({},
            "Cannot find symbol %s in environment of type %s", symbol, getExpressionName(expression.type));
    }
    const auto &dictionary = storage.evaluated_dictionaries.data[expression.index];
    if (steps == 0) {
        return requiredLookup(dictionary, name.global_index);
    }
    return lookupDictionarySecondTime(name, steps - 1, dictionary.environment);
}

Expression lookupDictionary(BoundGlobalName& name, Expression expression) {
    if (name.parent_steps == -1) {
        const auto result = lookupDictionaryFirstTime(name, 0, expression);
        if (isError(result.expression.type)) {
            return result.expression;
        }
        name.parent_steps = result.steps;
        return result.expression;
    }
    else {
        return lookupDictionarySecondTime(name, name.parent_steps, expression);
    }
}

Expression applyFunctionBuiltIn(
    Expression function, Expression input
) {
    const auto function_struct = storage.built_in_functions.data[function.index];
    return function_struct.function(input);
}

struct BooleanResult {
    bool value;
    Expression error;
};

BooleanResult booleanTypes(Expression expression) {
    auto result = MAKE(BooleanResult);
    switch (expression.type) {
        case PARSE_ERROR: return result;
        case NUMBER: return result;
        case YES: return result;
        case NO: return result;
        case EVALUATED_TABLE: return result;
        case EVALUATED_STACK: return result;
        case EMPTY_STACK: return result;
        case STRING: return result;
        case EMPTY_STRING: return result;
        case ANY: return result;
        default:
            return MAKE(BooleanResult, .error=makeEvaluateError(expression.range,
                "Static type error.\n"
                "Cannot convert type %s to boolean.",
                getExpressionName(expression.type)
            ));
    }
}
    
BooleanResult boolean(Expression expression) {
    const auto type = expression.type;
    const auto index = expression.index;
    switch (type) {
    case PARSE_ERROR: return MAKE(BooleanResult, .value=false);
    case EVALUATED_TABLE: return MAKE(BooleanResult, .value=!storage.evaluated_tables.at(index).empty());
    case EVALUATED_TABLE_VIEW: return MAKE(BooleanResult, .value=!storage.evaluated_table_views.data[index].empty());
    case NUMBER: return MAKE(BooleanResult, .value=static_cast<bool>(getNumber(expression)));
    case YES: return MAKE(BooleanResult, .value=true);
    case NO: return MAKE(BooleanResult, .value=false);
    case EVALUATED_STACK: return MAKE(BooleanResult, .value=true);
    case EMPTY_STACK: return MAKE(BooleanResult, .value=false);
    case STRING: return MAKE(BooleanResult, .value=true);
    case EMPTY_STRING: return MAKE(BooleanResult, .value=false);
    default: return MAKE(BooleanResult, .error=makeEvaluateError(expression.range,
        "I found an error while trying to evaluate a boolean expression.\n"
        "I got an unexpected type %s.", getExpressionName(type)));
    }
}

Expression applyTupleIndexing(Expression tuple, Expression input) {
    const auto& tuple_struct = storage.evaluated_tuples.data[tuple.index];
    if (input.type != NUMBER) {
        return makeEvaluateError(tuple.range,
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
        return makeEvaluateError(tuple.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto i = (size_t)number;
    const auto count = tuple_struct.indices.count;
    if (i >= count) {
        return makeEvaluateError(tuple.range,
            "Tuple of size %zu indexed with %zu" , count, i
        );
    }
    return storage.expressions.data[tuple_struct.indices.data + i];
}

Expression applyTableIndexingTypes(Expression table) {
    const auto& table_struct = storage.evaluated_tables.at(table.index);
    if (table_struct.rows.empty()) {
        return Expression{0, table.range, ANY};
    }
    return table_struct.begin()->second.value;
}

Expression applyStackIndexingTypes(Expression stack) {
    return storage.evaluated_stacks.data[stack.index].top;
}

Expression applyStringIndexingTypes(Expression string) {
    return storage.strings.data[string.index].top;
}

bool isEqual(Expression left, Expression right);

bool isTuplePairwiseEqual(EvaluatedTuple left, EvaluatedTuple right) {
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

bool isStackPairwiseEqual(Expression left, Expression right) {
    while (left.type != EMPTY_STACK && right.type != EMPTY_STACK) {
        CHECK_INTERNAL(left.type == EVALUATED_STACK,
            "Internal error detected in isStackPairwiseEqual.\n"
            "Expected a stack but got a %s",
            getExpressionName(left.type)
        );
        CHECK_INTERNAL(right.type == EVALUATED_STACK,
            "Internal error detected in isStackPairwiseEqual. "
            "Expected a stack but got a %s",
            getExpressionName(right.type)
        );
        const auto left_container = storage.evaluated_stacks.data[left.index];
        const auto right_container = storage.evaluated_stacks.data[right.index];
        if (!isEqual(left_container.top, right_container.top)) {
            return false;
        }
        left = left_container.rest;
        right = right_container.rest;
    }
    return left.type == EMPTY_STACK && right.type == EMPTY_STACK;
}

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
    if (left_type == EVALUATED_STACK && right_type == EVALUATED_STACK) {
        return isStackPairwiseEqual(left, right);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return isStringPairwiseEqual(left, right);
    }
    if (left_type == EVALUATED_TUPLE && right_type == EVALUATED_TUPLE) {
        return isTuplePairwiseEqual(
            storage.evaluated_tuples.data[left.index],
            storage.evaluated_tuples.data[right.index]
        );
    }
    return false;
}

Expression evaluateDynamicExpressionTyped(Expression expression) {
    return Expression{0, expression.range, ANY};
}

Expression evaluateDynamicExpression(Expression expression, Expression environment) {
    const auto inner_expression = storage.dynamic_expressions.data[expression.index].expression;
    return evaluate(inner_expression, environment);
}

Expression evaluateConditionalTypes(
    Expression conditional, Expression environment
) {
    const auto conditional_struct = storage.conditionals.data[conditional.index];
    FOR_EACH(a, conditional_struct.alternatives) {
        evaluate_types(storage.alternatives.data[a].left, environment);
    }
    const auto else_expression = evaluate_types(conditional_struct.expression_else, environment);
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

Expression evaluateConditional(Expression conditional, Expression environment) {
    const auto conditional_struct = storage.conditionals.data[conditional.index];
    FOR_EACH(a, conditional_struct.alternatives) {
        const auto alternative = storage.alternatives.data[a];
        const auto condition = boolean(evaluate(alternative.left, environment));
        if (isError(condition.error.type)) {
            return condition.error;
        }
        if (condition.value) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(conditional_struct.expression_else, environment);
}

Expression evaluateIsTypes(
    Expression is, Expression environment
) {
    const auto is_struct = storage.is_expressions.data[is.index];
    evaluate_types(is_struct.input, environment);
    FOR_EACH(a, is_struct.alternative) {
        const auto alternative = storage.alternatives.data[a];
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(is_struct.expression_else, environment);
    FOR_EACH(a, is_struct.alternative) {
        const auto alternative = storage.alternatives.data[a];
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        auto type_check = checkTypes(else_expression, alternative_expression, "is");
        if (!type_check.ok) return type_check.error;
    }
    return else_expression;
}

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

template<typename Evaluator>
Expression evaluateTypedExpression(
    Evaluator evaluator, Expression expression, Expression environment
) {
    const auto type = lookupDictionary(
        storage.typed_expressions.data[expression.index].type_name, environment
    );
    const auto value = evaluator(storage.typed_expressions.data[expression.index].value, environment);
    checkTypes(type, value, "typed expression");
    return value;
}

Indices initializeDefinitions(const Dictionary& dictionary) {
    // TODO: allocate on storage.expressions directly.
    // Allocation:
    auto first = storage.definitions.count;
    auto definitions = Indices{first, dictionary.definition_count};
    FOR_EACH(i, definitions) {
        makeDefinition(CodeRange{}, Definition{});
    }
    FOR_EACH(i, dictionary.statements) {
        auto statement = storage.statements.data[i];
        auto type = statement.type;
        if (type == DEFINITION) {
            auto definition = storage.definitions.data[statement.index];
            definition.expression = Expression{0, statement.range, ANY};
            auto dictionary_index = definition.name.dictionary_index;
            storage.definitions.data[first + dictionary_index] = definition;
        }
        else if (type == FOR_STATEMENT) {
            auto for_statement = storage.for_statements.data[statement.index];
            auto dictionary_index = for_statement.item_name.dictionary_index;
            auto definition = Definition{
                for_statement.item_name, Expression{0, statement.range, ANY}
            };
            storage.definitions.data[first + dictionary_index] = definition;
        }
    }
    auto last = storage.definitions.count;
    return Indices{first, last - first};
}

void setDictionaryDefinition(
    Expression evaluated_dictionary, BoundLocalName name, Expression value
) {
    CHECK_INTERNAL(
        evaluated_dictionary.type == EVALUATED_DICTIONARY,
        "setDictionaryDefinition expected %s got %s",
        getExpressionName(EVALUATED_DICTIONARY),
        getExpressionName(evaluated_dictionary.type)
    );
    auto first = storage.evaluated_dictionaries.data[evaluated_dictionary.index].definitions.data;
    storage.definitions.data[first + name.dictionary_index].expression = value;
}

Expression getDictionaryDefinition(
    Expression evaluated_dictionary, BoundLocalName name
) {
    CHECK_INTERNAL(
        evaluated_dictionary.type == EVALUATED_DICTIONARY,
        "getDictionaryDefinition expected %s got %s",
        getExpressionName(EVALUATED_DICTIONARY),
        getExpressionName(evaluated_dictionary.type)
    );
    auto first = storage.evaluated_dictionaries.data[evaluated_dictionary.index].definitions.data;
    return storage.definitions.data[first + name.dictionary_index].expression;
}

Expression evaluateDictionaryTypes(
    Expression dictionary, Expression environment
) {
    // TODO: allocate on storage.definitions directly.
    const auto initial_definitions = initializeDefinitions(
        storage.dictionaries.data[dictionary.index]
    );
    const auto result = makeEvaluatedDictionary(
        dictionary.range, EvaluatedDictionary{environment, initial_definitions}
    );
    const auto dictionary_struct = storage.dictionaries.data[dictionary.index];
    FOR_EACH(i, dictionary_struct.statements) {
        const auto statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = storage.definitions.data[statement.index];
            const auto right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result);
            // TODO: is this a principled approach?
            if (value.type != ANY) {
                setDictionaryDefinition(result, definition.name, value);
            }
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = storage.put_assignments.data[statement.index];
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result);
            const auto current = getDictionaryDefinition(result, put_assignment.name);
            const auto tuple = makeEvaluatedTuple2(value, current);
            const auto new_value = container_functions::putTyped(tuple);
            setDictionaryDefinition(result, put_assignment.name, new_value);
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            const auto put_each_assignment = storage.put_each_assignments.data[statement.index];
            const auto right_expression = put_each_assignment.expression;
            auto container = evaluate_types(right_expression, result);
            {
                const auto current = getDictionaryDefinition(result, put_each_assignment.name);
                const auto value = container_functions::takeTyped(container);
                const auto tuple = makeEvaluatedTuple2(value, current);
                const auto new_value = container_functions::putTyped(tuple);
                setDictionaryDefinition(result, put_each_assignment.name, new_value);
            }
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = storage.drop_assignments.data[statement.index];
            const auto current = getDictionaryDefinition(result, drop_assignment.name);
            const auto new_value = container_functions::dropTyped(current);
            setDictionaryDefinition(result, drop_assignment.name, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.data[statement.index];
            auto condition = booleanTypes(evaluate_types(while_statement.expression, result));
            if (isError(condition.error.type)) return condition.error;
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = storage.for_statements.data[statement.index];
            const auto container = getDictionaryDefinition(result, for_statement.container_name);
            auto condition = booleanTypes(container);
            if (isError(condition.error.type)) return condition.error;
            const auto value = container_functions::takeTyped(container);
            setDictionaryDefinition(result, for_statement.item_name, value);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            const auto for_statement = storage.for_simple_statements.data[statement.index];
            const auto container = getDictionaryDefinition(result, for_statement.container_name);
            auto condition = booleanTypes(container);
            if (isError(condition.error.type)) return condition.error;
        }
        else if (type == RETURN_STATEMENT) {
        }
    }
    return result;
}

Expression evaluateDictionary(Expression dictionary, Expression environment) {
    const auto initial_definitions = initializeDefinitions(
        storage.dictionaries.data[dictionary.index]
    );
    const auto result = makeEvaluatedDictionary(
        dictionary.range, EvaluatedDictionary{environment, initial_definitions}
    );

    const auto dict_statements = storage.dictionaries.data[dictionary.index].statements;
    const auto base_index = dict_statements.data;
    const auto statement_count = dict_statements.count;
    
    auto i = size_t{0};
    while (i < statement_count) {
        const auto statement = *(storage.statements.data + base_index + i);
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = storage.definitions.data[statement.index];
            const auto right_expression = definition.expression;
            const auto value = evaluate(right_expression, result);
            // Bottleneck:
            setDictionaryDefinition(result, definition.name, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = storage.put_assignments.data[statement.index];
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate(right_expression, result);
            const auto current = getDictionaryDefinition(result, put_assignment.name);
            const auto tuple = makeEvaluatedTuple2(value, current);
            const auto new_value = container_functions::put(tuple);
            setDictionaryDefinition(result, put_assignment.name, new_value);
            i += 1;
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            const auto put_each_assignment = storage.put_each_assignments.data[statement.index];
            const auto right_expression = put_each_assignment.expression;
            auto container = evaluate(right_expression, result);
            for (;;) {
                auto condition = boolean(container);
                if (isError(condition.error.type)) {
                    return condition.error;
                }
                if (!condition.value) {
                    break;
                }
                const auto current = getDictionaryDefinition(result, put_each_assignment.name);
                const auto value = container_functions::take(container);
                const auto tuple = makeEvaluatedTuple2(value, current);
                const auto new_value = container_functions::put(tuple);
                setDictionaryDefinition(result, put_each_assignment.name, new_value);
                container = container_functions::drop(container);
            }
            i += 1;
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = storage.drop_assignments.data[statement.index];
            const auto current = getDictionaryDefinition(result, drop_assignment.name);
            const auto new_value = container_functions::drop(current);
            setDictionaryDefinition(result, drop_assignment.name, new_value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.data[statement.index];
            auto condition = boolean(evaluate(while_statement.expression, result));
            if (isError(condition.error.type)) {
                return condition.error;
            }
            if (condition.value) {
                i += 1;
            } else {
                i = while_statement.end_index + 1;
            }
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = storage.for_statements.data[statement.index];
            const auto container = getDictionaryDefinition(result, for_statement.container_name);
            auto condition = boolean(container);
            if (isError(condition.error.type)) {
                return condition.error;
            }
            if (condition.value) {
                const auto value = container_functions::take(container);
                setDictionaryDefinition(result, for_statement.item_name, value);
                i += 1;
            } else {
                i = for_statement.end_index + 1;
            }
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            const auto for_statement = storage.for_simple_statements.data[statement.index];
            const auto condition = boolean(getDictionaryDefinition(result, for_statement.container_name));
            if (isError(condition.error.type)) {
                return condition.error;
            }
            if (condition.value) {
                i += 1;
            } else {
                i = for_statement.end_index + 1;
            }
        }
        else if (type == WHILE_END_STATEMENT) {
            const auto end_statement = storage.while_end_statements.data[statement.index];
            i = end_statement.start_index;
        }
        else if (type == FOR_END_STATEMENT) {
            const auto end_statement = storage.for_end_statements.data[statement.index];
            i = end_statement.start_index;
            const auto start_statement = *(storage.statements.data + base_index + i);
            const auto name_index = storage.for_statements.data[start_statement.index].container_name;
            const auto old_container = getDictionaryDefinition(result, name_index);
            const auto new_container = container_functions::drop(old_container);
            setDictionaryDefinition(result, name_index, new_container);
        }
        else if (type == FOR_SIMPLE_END_STATEMENT) {
            const auto end_statement = storage.for_simple_end_statements.data[statement.index];
            i = end_statement.start_index;
            const auto start_statement = *(storage.statements.data + base_index + i);
            const auto name_index = storage.for_simple_statements.data[start_statement.index].container_name;
            const auto old_container = getDictionaryDefinition(result, name_index);
            const auto new_container = container_functions::drop(old_container);
            setDictionaryDefinition(result, name_index, new_container);
        }
        else if (type == RETURN_STATEMENT) {
            break;
        }
    }
    return result;
}

std::string stdStringFromManglang(Expression key) {
    auto buffer = StringBuilder{};
    buffer = serialize(buffer, key);
    auto k = makeStdString(buffer);
    FREE_DARRAY(buffer);
    return k;
}
    
Expression applyTableIndexing(Expression table, Expression key) {
    const auto& table_struct = storage.evaluated_tables.at(table.index);
    const auto& rows = table_struct.rows;
    auto k = stdStringFromManglang(key);
    auto it = rows.find(k);
    if (it == rows.end()) {
        return makeEvaluateError(table.range, "Cannot find key %s in table", k.c_str());
    }
    return it->second.value;
}

Expression applyStackIndexing(Expression stack, Expression input) {
    if (input.type != NUMBER) {
        return makeEvaluateError(stack.range,
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
        return makeEvaluateError(stack.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto index = (size_t)number;
    auto stack_struct = storage.evaluated_stacks.data[stack.index];
    for (size_t i = 0; i < index; ++i) {
        if (stack_struct.rest.type == EMPTY_STACK) {
            return makeEvaluateError(stack.range,
                "Stack index out of range"
            );
        }
        if (stack_struct.rest.type != EVALUATED_STACK) {
            return makeEvaluateError(stack.range,
                "I found a type error while indexing a stack. \n"
                "Instead of a stack I encountered a %s",
                getExpressionName(stack_struct.rest.type)
            );
        }
        stack_struct = storage.evaluated_stacks.data[stack_struct.rest.index];
    }
    return stack_struct.top;
}

Expression applyStringIndexing(Expression string, Expression input) {
    if (input.type != NUMBER) {
        return makeEvaluateError(string.range,
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
        return makeEvaluateError(string.range,
            "Cannot have negative index: %f", number
        );
    }
    const auto index = (size_t)number;
    auto string_struct = storage.strings.data[string.index];
    for (size_t i = 0; i < index; ++i) {
        if (string_struct.rest.type == EMPTY_STACK) {
            return makeEvaluateError(string.range,
                "String index out of range"
            );
        }
        if (string_struct.rest.type != STRING) {
            return makeEvaluateError(string.range,
                "I found a type error while indexing a string. \n"
                "Instead of a string I encountered a %s",
                getExpressionName(string_struct.rest.type)
            );
        }
        string_struct = storage.strings.data[string_struct.rest.index];
    }
    return string_struct.top;
}

Expression evaluateFunctionApplicationTypes(
    Expression function_application, Expression environment
) {
    const auto function = lookupDictionary(
        storage.function_applications.data[function_application.index].name, environment
    );
    const auto input = evaluate_types(
        storage.function_applications.data[function_application.index].child,
        environment
    );
    switch (function.type) {
        case PARSE_ERROR: return function;

        case FUNCTION: return applyFunction(evaluate_types, function, input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(function, input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate_types, function, input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate_types, function, input);

        case EVALUATED_TABLE: return applyTableIndexingTypes(function);
        case EVALUATED_TUPLE: return applyTupleIndexing(function, input);
        case EVALUATED_STACK: return applyStackIndexingTypes(function);
        case STRING: return applyStringIndexingTypes(function);

        case EMPTY_STACK: return Expression{0, function_application.range, ANY};
        case EMPTY_STRING: return Expression{0, function_application.range, CHARACTER};
    
        default: return makeEvaluateError(function_application.range,
            "I found an error during type checking.\n"
            "The application operator (!) received an %s, which I did not expect.",
            getExpressionName(function_application.type)
        );
    }
}

Expression evaluateFunctionApplication(
    Expression function_application, Expression environment
) {
    const auto function = lookupDictionary(
        storage.function_applications.data[function_application.index].name,
        environment
    );
    const auto input = evaluate(
        storage.function_applications.data[function_application.index].child,
        environment
    );
    switch (function.type) {
        case PARSE_ERROR: return function;

        case FUNCTION: return applyFunction(evaluate, function, input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(function, input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate, function, input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate, function, input);
        
        case EVALUATED_TABLE: return applyTableIndexing(function, input);
        case EVALUATED_TUPLE: return applyTupleIndexing(function, input);
        case EVALUATED_STACK: return applyStackIndexing(function, input);
        case STRING: return applyStringIndexing(function, input);
        
        case EMPTY_STACK: return makeEvaluateError(function_application.range,
            "I caught a run-time error when trying to index an empty stack.");
        case EMPTY_STRING: return makeEvaluateError(function_application.range,
            "I caught a run-time error when trying to index an empty string.");

        default: return makeEvaluateError(function_application.range,
            "I found an error during evaluation.\n"
            "The application operator (!) received an %s, which I did not expect.",
            getExpressionName(function_application.type)
        );
    }
}

} // namespace

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case PARSE_ERROR: return expression;
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(expression, environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression, environment);
        case LOOKUP_SYMBOL: return lookupDictionary(storage.symbol_lookups.data[expression.index].name, environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE: return evaluateTuple(evaluate_types, expression, environment);
        case TABLE: return evaluateTable(evaluate_types, serialize_types, expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate_types, expression, environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate_types, expression, environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpressionTyped(expression);
        case CONDITIONAL: return evaluateConditionalTypes(expression, environment);
        case IS: return evaluateIsTypes(expression, environment);
        case DICTIONARY: return evaluateDictionaryTypes(expression, environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplicationTypes(expression, environment);
    
        default: return makeEvaluateError(expression.range,
            "I found an error during type checking.\n"
            "I received an %s, which I did not expect.",
            getExpressionName(expression.type)
        );
    }
}

Expression evaluate(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case PARSE_ERROR: return expression;
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(expression, environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression, environment);
        case LOOKUP_SYMBOL: return lookupDictionary(storage.symbol_lookups.data[expression.index].name, environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate, expression, environment);
        case TUPLE: return evaluateTuple(evaluate, expression, environment);
        case TABLE: return evaluateTable(evaluate, serialize, expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate, expression, environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate, expression, environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpression(expression, environment);
        case CONDITIONAL: return evaluateConditional(expression, environment);
        case IS: return evaluateIs(expression, environment);
        case DICTIONARY: return evaluateDictionary(expression, environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(expression, environment);
    
        default: return makeEvaluateError(expression.range,
            "I found an error during evaluation.\n"
            "I received an %s, which I did not expect.",
            getExpressionName(expression.type)
        );
    }
}
