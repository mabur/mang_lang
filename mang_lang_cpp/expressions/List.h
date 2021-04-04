#pragma once
#include "Expression.h"

struct List : public Expression {
    List(CodeRange range, const Expression* parent, InternalList elements)
        : Expression{range, parent}, elements{std::move(elements)} {}
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    const InternalList& list() const final;
    bool boolean() const final;
    bool isEqual(const Expression* expression) const final;

    ExpressionPointer first() const final;
    ExpressionPointer rest() const final;
    ExpressionPointer reverse() const final;
    ExpressionPointer prepend(ExpressionPointer item) const final;

private:
    InternalList elements;
};
