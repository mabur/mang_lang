#pragma once

#include "../expressions/Expression.h"

ExpressionPointer apply(const Expression* expression, ExpressionPointer input, std::ostream& log);
