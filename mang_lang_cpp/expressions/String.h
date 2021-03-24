#pragma once
#include "Expression.h"

struct String : public Expression {
    String(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string value
    ) : Expression{first, last, parent}, value{std::move(value)} {}
    std::string value;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    bool boolean() const final;
};
