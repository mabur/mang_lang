#pragma once
#include "Expression.h"
#include "../SinglyLinkedList.h"

struct String : public Expression {
    String(CodeRange range, const Expression* parent, InternalList elements)
        : Expression{range, parent}, elements{std::move(elements)} {}
    InternalList elements;
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
