#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    explicit FunctionBuiltIn(std::function<ExpressionPointer(const ExpressionPointer&)> f)
        : Expression{{}, {}}, function(std::move(f)) {}
    std::function<ExpressionPointer(const ExpressionPointer&)> function;
};
