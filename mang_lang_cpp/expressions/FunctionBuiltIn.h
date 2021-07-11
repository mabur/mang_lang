#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    explicit FunctionBuiltIn(std::function<ExpressionPointer(const Expression&)> f)
        : Expression{{}, {}, FUNCTION_BUILT_IN}, function(std::move(f)) {}
    std::function<ExpressionPointer(const Expression&)> function;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
};
