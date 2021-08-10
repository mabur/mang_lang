#pragma once
#include "Expression.h"

struct Number {
    CodeRange range;
    ExpressionPointer environment;
    double value;
};
