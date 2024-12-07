#pragma once

#include <stdexcept>
#include <string>

#include <carma/carma_string.h>

StringView makeStaticString(const char* s);
StringBuilder makeStringBuilder(char* s);
std::string makeStdString(StringBuilder s);
StringBuilder concatenate(StringBuilder base, const char* tail);
StringBuilder readTextFile(const char* file_path);
