#include "evaluate_generic.h"

#include "serialize.h"

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

Expression lookupDictionary(const Name& name, Expression expression) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(name, "environment of type " + NAMES[expression.type]);
    }
    const auto dictionary = getEvaluatedDictionary(expression);
    const auto value = dictionary.definitions.lookup(name);
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

size_t getIndex(Number number) {
    if (number < 0) {
        using namespace std;
        throw runtime_error("Cannot have negative index: " + to_string(number));
    }
    return static_cast<size_t>(number);
}

Expression applyTupleIndexing(const EvaluatedTuple& tuple, Number number) {
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

Expression applyStackIndexing(EvaluatedStack stack, Number number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        if (stack.rest.type != EMPTY_STACK) {
            stack = getEvaluatedStack(stack.rest);
        }
        else {
            return Expression{ANY, {}, {}};   
        }
    }
    return stack.top;
}

Expression applyStringIndexing(String string, Number number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        if (string.rest.type != EMPTY_STACK) {
            string = getString(string.rest);
        }
        else {
            return Expression{ANY, {}, {}};
        }
    }
    return string.top;
}
