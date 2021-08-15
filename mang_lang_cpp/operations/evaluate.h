#pragma once

#include <ostream>

struct ExpressionPointer;

ExpressionPointer evaluate(
    ExpressionPointer expression, ExpressionPointer environment, std::ostream& log
);
