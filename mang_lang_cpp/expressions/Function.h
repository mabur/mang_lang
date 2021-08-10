#pragma once
#include "Expression.h"
#include "Name.h"

struct Function {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer input_name;
    ExpressionPointer body;
};
