#pragma once

#include <stdexcept>

#include "abstract_syntax_tree.h"
#include "parse_utils.h"


std::unique_ptr<Expression> parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
);

std::unique_ptr<Expression> parse(const std::string& string);
