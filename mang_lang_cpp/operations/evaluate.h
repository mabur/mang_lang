#pragma once

#include "../expressions/Expression.h"

ExpressionPointer evaluate(
    const ExpressionPointer& expression,
    const ExpressionPointer& environment,
    std::ostream& log
);
