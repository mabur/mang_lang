#pragma once

#include <stdexcept>

#include "abstract_syntax_tree.h"
#include "parse_utils.h"


ExpressionPointer parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
);

ExpressionPointer parse(const std::string& string);
