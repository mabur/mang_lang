#pragma once

struct Expression;

Expression evaluate_types(Expression expression, Expression environment);
Expression evaluate(Expression expression, Expression environment);
