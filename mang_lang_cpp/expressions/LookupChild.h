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
    virtual std::string serialize() const {
        return name + "<" + child->serialize();
    };
    virtual ExpressionPointer evaluate(const Expression* parent) const {
        const auto evaluated_child = child->evaluate(parent);
        return ExpressionPointer{evaluated_child->lookup(name)};
    }
};
