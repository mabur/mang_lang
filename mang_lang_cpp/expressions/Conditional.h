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
    virtual std::string serialize() const {
        return std::string{"if "} + expression_if->serialize()
            + " then " + expression_then->serialize()
            + " else " + expression_else->serialize();
    };
    virtual ExpressionPointer evaluate(const Expression* parent) const {
        if (expression_if->evaluate(parent)->isTrue()) {
            return expression_then->evaluate(parent);
        } else {
            return expression_else->evaluate(parent);
        }
    }
};
