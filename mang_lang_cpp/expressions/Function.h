#pragma once
#include "Expression.h"
#include "Name.h"

struct Function : public Expression {
    Function(
        CodeRange range,
        const Expression* environment,
        NamePointer input_name,
        ExpressionPointer body
    ) : Expression{range, environment},
        input_name{std::move(input_name)},
        body{std::move(body)}
    {}
    NamePointer input_name;
    ExpressionPointer body;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
