#pragma once
#include "Expression.h"
#include "ListBase.h"
#include "../SinglyLinkedList.h"

struct String : public ListBase<String> {
    String(CodeRange range, const Expression* environment, InternalList elements)
        : ListBase<String>{range, environment, std::move(elements)}{}
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
