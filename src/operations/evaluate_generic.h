#pragma once

#include <string>

struct Expression;
struct Function;
struct FunctionDictionary;
struct FunctionTuple;

std::string getNameAsLabel(Expression name);

Expression evaluateFunction(const Function& function, Expression environment);

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
);

Expression evaluateFunctionTuple(
    const FunctionTuple& function_stack, Expression environment
);
