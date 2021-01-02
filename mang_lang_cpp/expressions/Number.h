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
    virtual std::string serialize() const {
        std::stringstream s;
        s << value;
        return s.str();
    };
    virtual ExpressionPointer evaluate(const Expression* parent) const {
        return std::make_shared<Number>(begin(), end(), parent, value);
    }
    virtual bool isTrue() const {
        return static_cast<bool>(value);
    }
};
