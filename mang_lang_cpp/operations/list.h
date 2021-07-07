#pragma once

#include "../expressions/Expression.h"

struct List;
struct String;

const InternalList& list(const Expression* expression);

ExpressionPointer empty(const Expression* expression);

ExpressionPointer prepend(const Expression* expression, ExpressionPointer item);
