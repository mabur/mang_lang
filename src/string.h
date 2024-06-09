#pragma once

#include <stddef.h>
#include <string>

struct CString {
    const char* data;
    size_t count;
};

struct SerializedString {
    char* data;
    size_t count;
    size_t capacity;
};

std::string makeStdString(SerializedString s);
