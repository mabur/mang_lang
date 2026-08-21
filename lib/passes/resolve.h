#pragma once

struct Expression;

Expression resolve(Expression expression, Expression outer_scope);

