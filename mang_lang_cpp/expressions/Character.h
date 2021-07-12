#pragma once
#include "Expression.h"

struct Character : public Expression {
    Character(CodeRange range, const Expression* environment, char value)
        : Expression{range, environment, CHARACTER}, value{value} {}
    char value;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
};
