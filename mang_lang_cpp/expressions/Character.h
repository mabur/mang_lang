#pragma once
#include "Expression.h"

struct Character : public Expression {
    Character(CodeRange range, const Expression* environment, char value)
        : Expression{range, environment}, value{value} {}
    char value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    static bool startsWith(CodeRange code);
    bool isEqual(const Expression* expression) const final;
    char character() const final;
};
