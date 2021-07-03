#pragma once

#include "../expressions/Expression.h"

struct Character;

ExpressionPointer evaluate(const Character& character, const Expression* environment, std::ostream& log);
