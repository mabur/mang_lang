#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupChild {
    CodeRange range;
    NamePointer name;
    ExpressionPointer child;
};
