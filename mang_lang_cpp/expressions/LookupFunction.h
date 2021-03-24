#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupFunction : public Expression {
    LookupFunction(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        Name name,
        ExpressionPointer child
    ) : Expression{first, last, parent}, name{std::move(name)}, child{std::move(child)} {}
    Name name;
    ExpressionPointer child;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
