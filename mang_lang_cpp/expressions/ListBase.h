#pragma once
#include "Expression.h"

#include "../operations/boolean.h"
#include "../operations/is_equal.h"
#include "../operations/list.h"
#include "../operations/lookup.h"

template<typename T>
struct ListBase : public Expression {
    using value_type = T;

    ListBase(CodeRange range, const Expression* environment, InternalList elements)
        : Expression{range, environment, LIST_BASE}, elements{std::move(elements)}
    {}
    ExpressionPointer lookup(const std::string& name) const final {
        return ::lookup(*this, name);
    }
    const InternalList& list() const final {
        return ::listListBase(*this);
    }
    bool boolean() const final {
        return ::boolean(*this);
    }
    bool isEqual(const Expression* expression) const final {
        return ::isEqual(*this, expression);
    }
    ExpressionPointer empty() const final {
        return ::emptyListBase(*this);
    }
    ExpressionPointer prepend(ExpressionPointer item) const final {
        return ::prependListBase(*this, item);
    }

    InternalList elements;
};
