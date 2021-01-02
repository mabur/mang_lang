#pragma once

#include <stdexcept>

#include "expressions/Expression.h"
#include "parse_utils.h"


ExpressionPointer parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
);

ExpressionPointer parse(const std::string& string);
