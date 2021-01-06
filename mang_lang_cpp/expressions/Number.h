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
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;

    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);

    virtual double number() const;
    virtual bool boolean() const;
};
