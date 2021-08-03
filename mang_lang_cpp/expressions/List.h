#pragma once
#include "Expression.h"

#include "../operations/boolean.h"
#include "../operations/is_equal.h"
#include "../operations/empty.h"
#include "../operations/list.h"
#include "../operations/prepend.h"
#include "../operations/lookup.h"

struct List : public Expression {
    List(CodeRange range, ExpressionPointer environment, InternalList elements)
        : Expression{range, environment}, elements{std::move(elements)}
    {}
    InternalList elements;
};
