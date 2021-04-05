#pragma once
#include "Expression.h"
#include "ListBase.h"

struct List : public ListBase<List> {
    List(CodeRange range, const Expression* parent, InternalList elements)
        : ListBase<List>{range, parent, std::move(elements)}{}
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
