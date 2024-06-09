#pragma once

#include <string>

struct Expression;

struct SerializedString {
    char* data;
    size_t count;
    size_t capacity;
};

inline std::string makeStdString(SerializedString s) {
    return std::string(s.data, s.count);
}

SerializedString serialize_types(SerializedString s, Expression expression);
SerializedString serialize(SerializedString s, Expression expression);
