#pragma once

#include <stdexcept>

#include "expressions/Expression.h"
#include "parse_utils.h"

ExpressionPointer parse(const std::string& string);
