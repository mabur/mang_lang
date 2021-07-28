#pragma once
#include "Expression.h"

struct Character : public Expression {
    Character(CodeRange range, ExpressionPointer environment, char value)
        : Expression{range, environment, CHARACTER}, value{value} {}
    char value;
};
