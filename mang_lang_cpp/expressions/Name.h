#pragma once
#include "Expression.h"

struct Name : public Expression {
    Name(CodeRange range, const Expression* parent, std::string value)
        : Expression{range, parent}, value{std::move(value)} {}
    std::string value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static Name parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
