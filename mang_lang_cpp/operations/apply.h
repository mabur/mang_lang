#pragma once

#include "../expressions/Expression.h"

struct Function;
struct FunctionDictionary;
struct FunctionList;

ExpressionPointer apply(const Function& function, ExpressionPointer input, std::ostream& log);
ExpressionPointer apply(const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log);
ExpressionPointer apply(const FunctionList& function_list, ExpressionPointer input, std::ostream& log);
