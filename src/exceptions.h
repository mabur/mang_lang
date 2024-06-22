#pragma once

#include <string>

struct CodeRange;

void throwException(const char* format, ...);

std::string describeLocation(CodeRange code);
