#pragma once
#include "Expression.h"

template<typename T>
struct ListBase : public Expression {
    ListBase(CodeRange range, const Expression* parent, InternalList elements)
        : Expression{range, parent}, elements{std::move(elements)}
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
    ExpressionPointer prepend(ExpressionPointer item) const final {
        auto new_list = ::prepend(list(), item);
        return std::make_shared<T>(range(), nullptr, new_list);
    }
private:
    InternalList elements;
};
