#pragma once

#include "../expressions/Expression.h"

ExpressionPointer lookup(const Expression* expression, const std::string& name);
