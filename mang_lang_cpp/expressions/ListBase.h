#pragma once
#include "Expression.h"

template<typename T>
struct ListBase : public Expression {
    ListBase(CodeRange range, const Expression* parent, InternalList elements)
        : Expression{range, parent}, elements{std::move(elements)}
    {}
    const InternalList& list() const final {
        return elements;
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
        return std::make_shared<T>(range(), nullptr, nullptr);
    }
    ExpressionPointer first() const final {
        return list()->first;
    }
    ExpressionPointer rest() const final {
        return std::make_shared<T>(range(), nullptr, list()->rest);
    }
    ExpressionPointer prepend(ExpressionPointer item) const final {
        auto new_list = ::prepend(list(), item);
        return std::make_shared<T>(range(), nullptr, new_list);
    }
private:
    InternalList elements;
};
