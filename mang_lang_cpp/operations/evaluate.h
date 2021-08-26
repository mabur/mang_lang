#pragma once

#include <ostream>

struct Expression;

Expression evaluate(
    Expression expression, Expression environment, std::ostream& log
);
