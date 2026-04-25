#pragma once

struct CodeRange;
struct Expression;

void throwException(const char* format, ...);
void throwMissingSymbolException(const char* symbol, Expression parent);

const char* describeLocation(CodeRange code);
