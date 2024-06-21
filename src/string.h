#pragma once

#include <stdexcept>
#include <string>

#include <carma/carma_string.h>

ConstantString makeStaticString(const char* s);
std::string makeStdString(DynamicString s);
DynamicString concatenate(DynamicString base, const char* tail);

static
inline
void throwException(const char* format, ...) {
    va_list args0;
    va_list args1;
    va_start(args0, format);
    va_copy(args1, args0);

    static auto string = DynamicString{};
    CLEAR(string);
    auto num_characters = vsnprintf(string.data, (size_t)string.capacity, format, args0);
    if (num_characters >= 0) {
        if (num_characters >= (int)string.capacity) {
            RESERVE(string, (size_t) num_characters + 1);
            num_characters = vsnprintf(string.data, string.capacity, format, args1);
        }
        string.count = (size_t)num_characters;
    }

    va_end(args0);
    va_end(args1);

    throw std::runtime_error(string.data);
}
