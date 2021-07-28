#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionList : public Expression {
    FunctionList(
        CodeRange range,
        ExpressionPointer environment,
        std::vector<NamePointer> input_names,
        ExpressionPointer body
    ) : Expression{range, environment, FUNCTION_LIST},
        input_names{std::move(input_names)},
        body{std::move(body)}
    {}
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};
