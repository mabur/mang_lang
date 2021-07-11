#pragma once
#include "Expression.h"
#include "Name.h"

struct Function : public Expression {
    Function(
        CodeRange range,
        const Expression* environment,
        NamePointer input_name,
        ExpressionPointer body
    ) : Expression{range, environment, FUNCTION},
        input_name{std::move(input_name)},
        body{std::move(body)}
    {}
    NamePointer input_name;
    ExpressionPointer body;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
};
