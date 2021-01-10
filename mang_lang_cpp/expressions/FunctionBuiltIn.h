#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    FunctionBuiltIn(std::function<ExpressionPointer(const Expression&)> f
    ) : Expression{{}, {}, {}}, function(std::move(f))
    {}
    std::function<ExpressionPointer(const Expression&)> function;
    ExpressionPointer apply(ExpressionPointer input) const final;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent) const final;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
