#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;
struct Dictionary;
struct Function;
struct FunctionDictionary;

ExpressionPointer evaluate(const Character& character, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Conditional& conditional, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Dictionary& dictionary, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Function& function, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const FunctionDictionary& function_dictionary, const Expression* environment, std::ostream& log);
