#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionList : public Expression {
    FunctionList(
        CodeRange range,
        const Expression* environment,
        std::vector<NamePointer> input_names,
        ExpressionPointer body
    ) : Expression{range, environment},
        input_names{std::move(input_names)},
        body{std::move(body)}
    {}
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
};
