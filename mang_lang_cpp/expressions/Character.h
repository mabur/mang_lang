#pragma once
#include "Expression.h"

struct Character : public Expression {
    Character(CodeRange range, const Expression* parent, char value)
        : Expression{range, parent}, value{value} {}
    char value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    char character() const final;
};
