#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    explicit FunctionBuiltIn(std::function<ExpressionPointer(const ExpressionPointer&)> f)
        : Expression{{}, {}, FUNCTION_BUILT_IN}, function(std::move(f)) {}
    std::function<ExpressionPointer(const ExpressionPointer&)> function;
};
