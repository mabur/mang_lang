#pragma once

#include "../expressions/Expression.h"

struct List;
struct String;

const InternalList& list(const Expression* expression);

ExpressionPointer emptyListBase(const List& list);
ExpressionPointer emptyListBase(const String& list);

ExpressionPointer prependListBase(const List& list, ExpressionPointer item);
ExpressionPointer prependListBase(const String& list, ExpressionPointer item);
