#pragma once

#include <string>

struct CodeRange;

void throwException(const char* format, ...);

const char* describeLocation(CodeRange code);
