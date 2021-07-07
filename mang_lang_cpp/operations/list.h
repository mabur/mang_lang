#pragma once

#include "../expressions/Expression.h"

struct List;
struct String;

const InternalList& list(const Expression* expression);

ExpressionPointer empty(const Expression* expression);

ExpressionPointer prependListBase(const List& list, ExpressionPointer item);
ExpressionPointer prependListBase(const String& list, ExpressionPointer item);
