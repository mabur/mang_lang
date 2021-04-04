#pragma once
#include "Expression.h"
#include "Name.h"

struct Function : public Expression {
    Function(
        CodeRange range,
        const Expression* parent,
        Name input_name,
        ExpressionPointer body
    ) : Expression{range, parent},
        input_name{std::move(input_name)},
        body{std::move(body)}
    {}
    Name input_name;
    ExpressionPointer body;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
