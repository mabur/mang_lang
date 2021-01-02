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
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(
        const CodeCharacter* first, const CodeCharacter* last
    );
    static bool isConditional(const CodeCharacter* first);
};
