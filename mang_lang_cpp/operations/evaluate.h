#pragma once

#include "../expressions/Expression.h"

ExpressionPointer evaluate(const Expression* expression, const Expression* environment, std::ostream& log);
