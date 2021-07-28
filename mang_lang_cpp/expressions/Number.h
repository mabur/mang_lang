#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(CodeRange range, const ExpressionPointer environment, double value)
        : Expression{range, environment, NUMBER}, value{value} {}
    double value;
};
