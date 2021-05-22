#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol : public Expression {
    LookupSymbol(
        CodeRange range,
        const Expression* parent,
        NamePointer name
    ) : Expression{range, parent}, name{std::move(name)} {}
    NamePointer name;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
