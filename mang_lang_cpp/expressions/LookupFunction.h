#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer name;
    ExpressionPointer child;
};
