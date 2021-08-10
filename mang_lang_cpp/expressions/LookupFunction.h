#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction {
    CodeRange range;
    NamePointer name;
    ExpressionPointer child;
};
