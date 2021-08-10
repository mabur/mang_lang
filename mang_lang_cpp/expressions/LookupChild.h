#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupChild {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer name;
    ExpressionPointer child;
};
