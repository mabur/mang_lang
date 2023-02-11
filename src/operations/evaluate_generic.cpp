#include "evaluate_generic.h"

#include "../factory.h"

std::string getNameAsLabel(Expression name) {
    return getName(name).value;
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

Expression lookupCurrentDictionary(Expression expression, const std::string& name) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(name, "environment of type " + NAMES[expression.type]);
    }
    const auto d = getEvaluatedDictionary(expression);
    return d.definitions.lookup(name);
}

Expression lookupDictionary(Expression expression, const std::string& name) {
    const auto result = lookupCurrentDictionary(expression, name);
    if (result.type != EMPTY) {
        return result;
    }
    const auto d = getEvaluatedDictionary(expression);
    return lookupDictionary(d.environment, name);
}

Expression evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, Expression environment
) {
    return lookupDictionary(environment, getNameAsLabel(lookup_symbol.name));
}
