#pragma once

#include "../expressions/Expression.h"

ExpressionPointer evaluate(
    ExpressionPointer expression,
    ExpressionPointer environment,
    std::ostream& log
);
