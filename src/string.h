#pragma once

#include <stdarg.h>
#include <string>

#include <carma/carma_string.h>

std::string makeStdString(StringBuilder s);
StringBuilder concatenate(StringBuilder base, const char* tail);

const char* format_cstring_v(const char* format, va_list args);
const char* format_cstring(const char* format, ...);
