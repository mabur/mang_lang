#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(CodeRange range, const Expression* environment, double value)
        : Expression{range, environment, NUMBER}, value{value} {}
    double value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;

    bool isEqual(const Expression* expression) const final;
    double number() const final;
    bool boolean() const final;
};
