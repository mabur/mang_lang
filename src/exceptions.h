#pragma once

struct CodeRange;

void throwException(const char* format, ...);

const char* describeLocation(CodeRange code);
