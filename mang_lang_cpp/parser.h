#pragma once

#include <stdexcept>

#include "abstract_syntax_tree.h"

struct ParseException : public std::runtime_error
{
    using runtime_error::runtime_error;
};

std::unique_ptr<Expression> parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
);

std::unique_ptr<Expression> parse(const std::string& string);
