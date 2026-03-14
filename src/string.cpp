#include "string.h"

#include <stdlib.h>

#include <carma/carma_string.h>

std::string makeStdString(StringBuilder s) {
    return std::string(s.data, s.count);
}

StringBuilder concatenate(StringBuilder base, const char* tail) {
    CONCAT_CSTRING(base, tail);
    return base;
}

const char* format_cstring(const char* format, ...)
{
    char* result = nullptr;
    va_list args1;
    va_start(args1, format);
    va_list args2;
    va_copy(args2, args1);

    int len = vsnprintf(nullptr, 0, format, args1);
    size_t count = (size_t)len + 1;
    if (len >= 0) {
        result = (char*)malloc(count);
        if (result) {
            int written = vsnprintf(result, count, format, args2);
            if (written < 0) {
                free(result);
                result = nullptr;
            }
        }
    }
    va_end(args1);
    va_end(args2);
    return result;
}
