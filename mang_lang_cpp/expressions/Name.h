#pragma once
#include "Expression.h"

struct Name;
using NamePointer = std::shared_ptr<const Name>;

struct Name : public Expression {
    Name(CodeRange range, const ExpressionPointer environment, std::string value)
        : Expression{range, environment, NAME}, value{std::move(value)} {}
    std::string value;
};
