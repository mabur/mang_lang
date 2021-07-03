#pragma once
#include "Expression.h"

#include "../operations/list.h"

template<typename T>
struct ListBase : public Expression {
    using value_type = T;

    ListBase(CodeRange range, const Expression* environment, InternalList elements)
        : Expression{range, environment}, elements{std::move(elements)}
    {}
    ExpressionPointer lookup(const std::string& name) const final {
        if (name == "first") {
            return list()->first;
        }
        if (name == "rest") {
            return std::make_shared<T>(range(), nullptr, list()->rest);
        }
        throw ParseException("List does not contain symbol " + name);
    }
    const InternalList& list() const final {
        return ::listListBase(*this);
    }
    bool boolean() const final {
        return !!list();
    }
    bool isEqual(const Expression* expression) const final {
        auto left = list();
        auto right = expression->list();
        for (; left && right; left = left->rest, right = right->rest) {
            if (!(left->first)->isEqual(right->first.get())) {
                return false;
            }
        }
        return !left && !right;
    }
    ExpressionPointer empty() const final {
        return ::emptyListBase(*this);
    }
    ExpressionPointer prepend(ExpressionPointer item) const final {
        return ::prependListBase(*this, item);
    }

    InternalList elements;
};
