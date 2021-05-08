#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionList : public Expression {
    FunctionList(
        CodeRange range,
        const Expression* parent,
        const std::vector<Name>& input_names,
        ExpressionPointer body
    ) : Expression{range, parent},
        input_names{input_names},
        body{std::move(body)}
    {}
    std::vector<Name> input_names;
    ExpressionPointer body;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
