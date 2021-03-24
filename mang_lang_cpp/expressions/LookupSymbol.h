#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupSymbol : public Expression {
    LookupSymbol(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        Name name
    ) : Expression{first, last, parent}, name{std::move(name)} {}
    Name name;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
