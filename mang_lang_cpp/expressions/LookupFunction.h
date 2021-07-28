#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction : public Expression {
    LookupFunction(
        CodeRange range,
        ExpressionPointer environment,
        NamePointer name,
        ExpressionPointer child
    ) : Expression{range, environment, LOOKUP_FUNCTION}, name{std::move(name)}, child{std::move(child)} {}
    NamePointer name;
    ExpressionPointer child;
};
