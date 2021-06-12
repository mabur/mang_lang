#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol : public Expression {
    LookupSymbol(
        CodeRange range,
        const Expression* environment,
        NamePointer name
    ) : Expression{range, environment}, name{std::move(name)} {}
    NamePointer name;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
