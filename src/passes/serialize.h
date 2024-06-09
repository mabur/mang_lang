#pragma once

#include <string>

struct Expression;
struct SerializedString;

SerializedString serialize_types(SerializedString s, Expression expression);
SerializedString serialize(SerializedString s, Expression expression);
