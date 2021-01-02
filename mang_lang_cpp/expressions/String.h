#pragma once
#include "Expression.h"

struct String : public Expression {
    String(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string value
    ) : Expression{first, last, parent}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return "\"" + value + "\"";
    };
    virtual ExpressionPointer evaluate(const Expression* parent) const {
        return std::make_shared<String>(begin(), end(), parent, value);
    }
};
