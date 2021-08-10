#pragma once
#include "Expression.h"

struct Conditional {
    CodeRange range;
    ExpressionPointer environment;
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
};
