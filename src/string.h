#pragma once

#include <stddef.h>
#include <string>

struct StaticString {
    const char* data;
    size_t count;
};

struct DynamicString {
    char* data;
    size_t count;
    size_t capacity;
};

StaticString makeStaticString(const char* s);
std::string makeStdString(DynamicString s);
DynamicString concatenate(DynamicString base, const char* tail);
