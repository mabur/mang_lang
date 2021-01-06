#pragma once
#include "Expression.h"

struct Name : public Expression {
    Name(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string value
    ) : Expression{first, last, parent}, value{value} {}
    std::string value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent) const final;
    static Name parse(const CodeCharacter* first, const CodeCharacter* last);
};
