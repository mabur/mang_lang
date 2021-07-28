#pragma once

#include "../expressions/Expression.h"

ExpressionPointer apply(const ExpressionPointer& expression, ExpressionPointer input, std::ostream& log);
