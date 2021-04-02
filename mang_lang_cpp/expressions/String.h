#pragma once
#include "Expression.h"
#include "../SinglyLinkedList.h"

using InternalString = SinglyLinkedList<char>;

struct String : public Expression {
    String(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        InternalString elements
    ) : Expression{first, last, parent}, elements{std::move(elements)} {}
    InternalString elements;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    bool boolean() const final;

    ExpressionPointer first() const final;
    ExpressionPointer rest() const final;
    ExpressionPointer reverse() const final;
    ExpressionPointer prepend(ExpressionPointer item) const final;
};
