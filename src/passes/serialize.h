#pragma once

#include <string>

struct Expression;

using SerializedString = std::string&;

inline std::string makeStdString(SerializedString s) {
    return s;
}

SerializedString serialize_types(SerializedString s, Expression expression);
SerializedString serialize(SerializedString s, Expression expression);
