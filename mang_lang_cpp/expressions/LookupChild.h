#pragma once
#include "Expression.h"

struct LookupChild : public Expression {
    LookupChild(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string name,
        ExpressionPointer child
    ) : Expression{first, last, parent}, name{name}, child{child} {}
    std::string name;
    ExpressionPointer child;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
};
