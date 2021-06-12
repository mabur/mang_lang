#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction : public Expression {
    LookupFunction(
        CodeRange range,
        const Expression* environment,
        NamePointer name,
        ExpressionPointer child
    ) : Expression{range, environment}, name{std::move(name)}, child{std::move(child)} {}
    NamePointer name;
    ExpressionPointer child;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
