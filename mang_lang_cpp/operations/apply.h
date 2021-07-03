#pragma once

#include <ostream>

#include "../expressions/Expression.h"

struct Function;

ExpressionPointer apply(const Function& function, ExpressionPointer input, std::ostream& log);
