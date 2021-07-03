#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(CodeRange range, const Expression* environment, double value)
        : Expression{range, environment}, value{value} {}
    double value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;

    static bool startsWith(CodeRange code);

    bool isEqual(const Expression* expression) const final;
    double number() const final;
    bool boolean() const final;
};
