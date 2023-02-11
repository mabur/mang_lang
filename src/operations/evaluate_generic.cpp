#include "evaluate_generic.h"

#include "serialize.h"

std::string getNameAsString(Expression name) {
    return getName(name);
}

Expression evaluateFunction(const Function& function, Expression environment) {
    return makeFunction(CodeRange{}, {
        environment, function.input_name, function.body
    });
}

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
) {
    return makeFunctionDictionary(CodeRange{}, {
        environment,
        function_dictionary.input_names,
        function_dictionary.body
    });
}

Expression evaluateFunctionTuple(
    const FunctionTuple& function_stack, Expression environment
) {
    return makeFunctionTuple(CodeRange{}, {
        environment, function_stack.input_names, function_stack.body
    });
}

Expression lookupDictionary(Expression name, Expression expression) {
    const auto label = getNameAsString(name);
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(label, "environment of type " + NAMES[expression.type]);
    }
    const auto dictionary = getEvaluatedDictionary(expression);
    const auto value = dictionary.definitions.lookup(label);
    if (value.type != EMPTY) {
        return value;
    }
    return lookupDictionary(name, dictionary.environment);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
) {
    return function_built_in.function(input);
}

size_t getIndex(const Number& number) {
    if (number.value < 0) {
        using namespace std;
        throw runtime_error("Cannot have negative index: " + to_string(number.value));
    }
    return static_cast<size_t>(number.value);
}

Expression applyTupleIndexing(const EvaluatedTuple& tuple, const Number& number) {
    const auto i = getIndex(number);
    try {
        return tuple.expressions.at(i);
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error(
            "Tuple of size " + std::to_string(tuple.expressions.size()) +
                " indexed with " + std::to_string(i)
        );
    }
}

Expression applyTableIndexing(const EvaluatedTable& table, Expression key) {
    const auto k = serialize(key);
    try {
        return table.rows.at(k).value;
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error("Table does not have key: " + k);
    }
}

Expression applyStackIndexing(EvaluatedStack stack, const Number& number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        stack = getEvaluatedStack(stack.rest);
    }
    return stack.top;
}

Expression applyStringIndexing(String string, const Number& number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        string = getString(string.rest);
    }
    return string.top;
}