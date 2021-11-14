#include "expression.h"

WrongExpression::WrongExpression(
    ExpressionType type, const std::string& location)
    : std::runtime_error("Unexpected expression " + NAMES[type] + " for " + location)
{}
