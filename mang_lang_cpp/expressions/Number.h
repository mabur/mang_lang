#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(CodeRange range, const Expression* environment, double value)
        : Expression{range, environment, NUMBER}, value{value} {}
    double value;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;

    bool isEqual(const Expression* expression) const final;
};
