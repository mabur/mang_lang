#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn {
    CodeRange range;
    ExpressionPointer environment;
    std::function<ExpressionPointer(ExpressionPointer)> function;
};
