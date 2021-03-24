#pragma once
#include "Expression.h"

struct Number : public Expression {
    Number(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        double value
    ) : Expression{first, last, parent}, value{value} {}
    double value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;

    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code_range);

    bool isEqual(const Expression* expression) const final;
    double number() const final;
    bool boolean() const final;
};
