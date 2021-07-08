#pragma once

#include "../expressions/Expression.h"

struct Function;
struct FunctionBuiltIn;
struct FunctionDictionary;
struct FunctionList;

ExpressionPointer applyFunction(const Function& function, ExpressionPointer input, std::ostream& log);
ExpressionPointer applyFunctionBuiltIn(const FunctionBuiltIn& function_built_in, ExpressionPointer input, std::ostream& log);
ExpressionPointer applyFunctionDictionary(const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log);
ExpressionPointer applyFunctionList(const FunctionList& function_list, ExpressionPointer input, std::ostream& log);
