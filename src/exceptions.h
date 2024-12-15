#pragma once

#include <string>

#include "expression_type.h"

struct CodeRange;
struct Expression;

void throwException(const char* format, ...);
void throwUnexpectedExpressionException(ExpressionType type, const char* location);
void throwMissingSymbolException(const char* symbol, Expression parent);

const char* describeLocation(CodeRange code);
