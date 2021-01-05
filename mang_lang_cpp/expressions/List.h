#pragma once
#include "Expression.h"

struct List : public Expression {
    List(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::vector<ExpressionPointer> elements
    ) : Expression{first, last, parent}, elements{std::move(elements)}
    {}
    std::vector<ExpressionPointer> elements;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
    const std::vector<ExpressionPointer>& list() const;
};
