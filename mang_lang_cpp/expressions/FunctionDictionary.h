#pragma once
#include "Expression.h"
#include "Name.h"

struct FunctionDictionary : public Expression {
    FunctionDictionary(
        CodeRange range,
        const Expression* environment,
        std::vector<NamePointer> input_names,
        ExpressionPointer body
    ) : Expression{range, environment, FUNCTION_DICTIONARY},
        input_names{std::move(input_names)},
        body{std::move(body)}
    {}
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};
