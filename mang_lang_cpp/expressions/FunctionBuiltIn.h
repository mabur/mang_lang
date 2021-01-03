#pragma once
#include <functional>
#include "Expression.h"
#include "Name.h"

struct FunctionBuiltIn : public Expression {
    FunctionBuiltIn(std::function<ExpressionPointer(const Expression&)> f
    ) : Expression{{}, {}, {}}, function(std::move(f))
    {}
    std::function<ExpressionPointer(const Expression&)> function;
    virtual ExpressionPointer apply(const Expression& input) const;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
