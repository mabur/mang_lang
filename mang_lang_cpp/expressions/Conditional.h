#pragma once
#include "Expression.h"

struct Conditional : public Expression {
    Conditional(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        ExpressionPointer expression_if,
        ExpressionPointer expression_then,
        ExpressionPointer expression_else
    )
        : Expression{first, last, parent},
        expression_if{std::move(expression_if)},
        expression_then{std::move(expression_then)},
        expression_else{std::move(expression_else)}
    {}
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(
        const CodeCharacter* first, const CodeCharacter* last
    );
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
