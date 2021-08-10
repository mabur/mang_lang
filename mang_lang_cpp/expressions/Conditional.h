#pragma once
#include "Expression.h"

struct Conditional {
    CodeRange range;
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
};
