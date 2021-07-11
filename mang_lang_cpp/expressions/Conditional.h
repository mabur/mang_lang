#pragma once
#include "Expression.h"

struct Conditional : public Expression {
    Conditional(
        CodeRange range,
        const Expression* environment,
        ExpressionPointer expression_if,
        ExpressionPointer expression_then,
        ExpressionPointer expression_else
    )
        : Expression{range, environment, CONDITIONAL},
        expression_if{std::move(expression_if)},
        expression_then{std::move(expression_then)},
        expression_else{std::move(expression_else)}
    {}
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
};
