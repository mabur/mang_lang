#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol : public Expression {
    LookupSymbol(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        Name name
    ) : Expression{first, last, parent}, name{name} {}
    Name name;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
