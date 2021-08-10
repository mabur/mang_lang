#pragma once
#include "Expression.h"

struct Character {
    CodeRange range;
    ExpressionPointer environment;
    char value;
};
