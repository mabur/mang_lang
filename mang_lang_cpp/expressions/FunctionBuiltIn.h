#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn {
    CodeRange range;
    std::function<ExpressionPointer(ExpressionPointer)> function;
};
