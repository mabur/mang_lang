#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    explicit FunctionBuiltIn(std::function<ExpressionPointer(const Expression&)> f
    ) : Expression{{}, {}, {}}, function(std::move(f))
    {}
    std::function<ExpressionPointer(const Expression&)> function;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
