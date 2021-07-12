#pragma once
#include "Expression.h"

#include "../operations/boolean.h"
#include "../operations/is_equal.h"
#include "../operations/empty.h"
#include "../operations/list.h"
#include "../operations/prepend.h"
#include "../operations/lookup.h"

struct List : public Expression {
    List(CodeRange range, const Expression* environment, InternalList elements)
        : Expression{range, environment, LIST}, elements{std::move(elements)}
    {}
    using value_type = List;

    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;

    ExpressionPointer lookup(const std::string& name) const final {
        return ::lookup(this, name);
    }
    bool isEqual(const Expression* expression) const final {
        return ::isEqual(this, expression);
    }

    InternalList elements;
};
