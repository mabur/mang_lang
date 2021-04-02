#pragma once
#include "Expression.h"

struct Character : public Expression {
    Character(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        char value
    ) : Expression{first, last, parent}, value{value} {}
    char value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
