#pragma once

#include <string>

struct Expression;

using SerializedString = std::string&;

SerializedString serialize_types(SerializedString s, Expression expression);
SerializedString serialize(SerializedString s, Expression expression);
