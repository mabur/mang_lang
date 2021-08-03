#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupChild : public Expression {
    LookupChild(
        CodeRange range,
        ExpressionPointer environment,
        NamePointer name,
        ExpressionPointer child
    ) : Expression{range, environment}, name{std::move(name)}, child{std::move(child)} {}
    NamePointer name;
    ExpressionPointer child;
};
