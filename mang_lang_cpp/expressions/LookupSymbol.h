#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol : public Expression {
    LookupSymbol(
        CodeRange range,
        ExpressionPointer environment,
        NamePointer name
    ) : Expression{range, environment, LOOKUP_SYMBOL}, name{std::move(name)} {}
    NamePointer name;
};
