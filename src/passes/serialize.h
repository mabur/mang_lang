#pragma once

#include <string>

struct Expression;
struct DynamicString;

DynamicString serialize_types(DynamicString s, Expression expression);
DynamicString serialize(DynamicString s, Expression expression);
