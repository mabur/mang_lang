#pragma once
#include "Expression.h"
#include "Name.h"

struct Function : public Expression {
    Function(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        Name input_name,
        ExpressionPointer body
    ) : Expression{first, last, parent},
        input_name{std::move(input_name)},
        body{std::move(body)}
    {}
    Name input_name;
    ExpressionPointer body;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent) const final;
    ExpressionPointer apply(ExpressionPointer input) const final;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
};
