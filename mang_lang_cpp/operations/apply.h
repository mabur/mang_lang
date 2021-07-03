#pragma once

#include "../expressions/Expression.h"

struct Function;
struct FunctionDictionary;

ExpressionPointer apply(const Function& function, ExpressionPointer input, std::ostream& log);
ExpressionPointer apply(const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log);
