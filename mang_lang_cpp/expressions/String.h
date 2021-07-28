#pragma once
#include "Expression.h"
#include "../SinglyLinkedList.h"

#include "../operations/boolean.h"
#include "../operations/is_equal.h"
#include "../operations/empty.h"
#include "../operations/list.h"
#include "../operations/prepend.h"
#include "../operations/lookup.h"

struct String : public Expression {
    String(CodeRange range, const ExpressionPointer environment, InternalList elements)
        : Expression{range, environment, STRING}, elements{std::move(elements)}
    {}
    using value_type = String;
    InternalList elements;
};
