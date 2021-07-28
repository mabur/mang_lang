#pragma once

#include "../expressions/Expression.h"

ExpressionPointer evaluate(const ExpressionPointer& expression, const Expression* environment, std::ostream& log);
