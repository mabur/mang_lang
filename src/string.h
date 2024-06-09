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

std::string makeStdString(DynamicString s);
