#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(CodeRange range, const ExpressionPointer environment, double value)
        : Expression{range, environment}, value{value} {}
    double value;
};
