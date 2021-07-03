#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;
struct Dictionary;

ExpressionPointer evaluate(const Character& character, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Conditional& conditional, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Dictionary& dictionary, const Expression* environment, std::ostream& log);
