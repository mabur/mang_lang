#pragma once
#include "Expression.h"
#include "Name.h"

struct Function : public Expression {
    Function(
        CodeRange range,
        ExpressionPointer environment,
        NamePointer input_name,
        ExpressionPointer body
    ) : Expression{range, environment},
        input_name{std::move(input_name)},
        body{std::move(body)}
    {}
    NamePointer input_name;
    ExpressionPointer body;
};
