#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction : public Expression {
    LookupFunction(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        Name name,
        ExpressionPointer child
    ) : Expression{first, last, parent}, name{name}, child{child} {}
    Name name;
    ExpressionPointer child;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
