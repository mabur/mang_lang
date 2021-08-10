#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionDictionary {
    CodeRange range;
    ExpressionPointer environment;
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};
