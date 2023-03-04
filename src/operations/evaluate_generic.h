#pragma once

#include <string>

#include "../container.h"
#include "../expression.h"
#include "../factory.h"

Expression evaluateFunction(const Function& function, Expression environment);

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
);

Expression evaluateFunctionTuple(
    const FunctionTuple& function_stack, Expression environment
);

template<typename Evaluator>
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    const auto op = [&](Expression rest, Expression top) -> Expression {
        const auto evaluated_top = evaluator(top, environment);
        return putEvaluatedStack(rest, evaluated_top);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyStack(code, {});
    const auto output = leftFold(init, stack, op, EMPTY_STACK, getStack);
    return reverseEvaluatedStack(code, output);
}

template<typename Evaluator>
Expression evaluateTuple(Evaluator evaluator, Tuple tuple, Expression environment) {
    auto evaluated_expressions = std::vector<Expression>{};
    evaluated_expressions.reserve(tuple.expressions.size());
    for (const auto& expression : tuple.expressions) {
        evaluated_expressions.push_back(evaluator(expression, environment));
    }
    const auto code = CodeRange{};
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
}

template<typename Evaluator, typename Serializer>
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Table table,
    Expression environment
) {
    auto rows = std::map<std::string, Row>{};
    for (const auto& row : table.rows) {
        const auto key = evaluator(row.key, environment);
        const auto value = evaluator(row.value, environment);
        const auto serialized_key = serializer(key);
        rows[serialized_key] = {key, value};
    }
    const auto code = CodeRange{};
    return makeEvaluatedTable(code, EvaluatedTable{rows});
}

template<typename Evaluator>
Expression evaluateLookupChild(
    Evaluator evaluator, const LookupChild& lookup_child, Expression environment
) {
    const auto child = evaluator(lookup_child.child, environment);
    const auto name = getName(lookup_child.name);
    const auto dictionary = getEvaluatedDictionary(child);
    return dictionary.definitions.lookup(name);
}

Expression lookupDictionary(const Name& name, Expression expression);

template<typename Evaluator>
Expression applyFunction(
    Evaluator evaluator,
    const Function& function,
    Expression input
) {
    auto definitions = Definitions{};
    const auto name = getName(function.input_name);
    definitions.add(name, input);
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluator(function.body, middle);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
);

template<typename Evaluator>
Expression applyFunctionDictionary(
    Evaluator evaluator,
    const FunctionDictionary& function_dictionary,
    Expression input
) {
    // TODO: pass along environment? Is some use case missing now?
    return evaluator(function_dictionary.body, input);
}

template<typename Evaluator>
Expression applyFunctionTuple(
    Evaluator evaluator,
    const FunctionTuple& function_stack,
    Expression input
) {
    auto tuple = getEvaluatedTuple(input);
    const auto& input_names = function_stack.input_names;
    if (input_names.size() != tuple.expressions.size()) {
        throw std::runtime_error{"Wrong number of input to function"};
    }
    auto definitions = Definitions{};
    const auto num_inputs = input_names.size();
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto name = getName(input_names[i]);
        const auto expression = tuple.expressions[i];
        definitions.add(name, expression);
    }
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function_stack.environment, definitions}
    );
    return evaluator(function_stack.body, middle);
}

Expression applyTupleIndexing(const EvaluatedTuple& tuple, Number number);
Expression applyTableIndexing(const EvaluatedTable& table, Expression key);
Expression applyStackIndexing(EvaluatedStack stack, Number number);
Expression applyStringIndexing(String string, Number number);

template<typename Evaluator>
Expression evaluateFunctionApplication(
    Evaluator evaluator,
    const FunctionApplication& function_application,
    Expression environment
) {
    const auto function = lookupDictionary(getName(function_application.name), environment);
    const auto input = evaluator(function_application.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluator, getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluator, getFunctionDictionary(function), input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluator, getFunctionTuple(function), input);
        case EVALUATED_TABLE: return applyTableIndexing(getEvaluatedTable(function), input);
        case EVALUATED_TUPLE: return applyTupleIndexing(getEvaluatedTuple(function), getNumber(input));
        case EVALUATED_STACK: return applyStackIndexing(getEvaluatedStack(function), getNumber(input));
        case STRING: return applyStringIndexing(getString(function), getNumber(input));
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplication");
    }
}
