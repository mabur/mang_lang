#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionList {
    CodeRange range;
    ExpressionPointer environment;
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};
