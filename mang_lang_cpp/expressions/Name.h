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
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static Name parse(const CodeCharacter* first, const CodeCharacter* last);
};
