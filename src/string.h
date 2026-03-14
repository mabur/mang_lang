#pragma once

#include <stdexcept>
#include <string>

#include <carma/carma_string.h>

std::string makeStdString(StringBuilder s);
StringBuilder concatenate(StringBuilder base, const char* tail);

const char* format_cstring(const char* format, ...);
