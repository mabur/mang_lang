#pragma once
#include "Expression.h"
#include "ListBase.h"

struct List : public ListBase<List> {
    List(CodeRange range, const Expression* environment, InternalList elements)
        : ListBase<List>{range, environment, std::move(elements)}{}
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
};
