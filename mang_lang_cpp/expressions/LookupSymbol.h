#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer name;
};
