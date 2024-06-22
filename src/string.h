#pragma once

#include <stdexcept>
#include <string>

#include <carma/carma_string.h>

ConstantString makeStaticString(const char* s);
std::string makeStdString(DynamicString s);
DynamicString concatenate(DynamicString base, const char* tail);
