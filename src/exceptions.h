#pragma once

#include <string>

#include "expression_type.h"

struct CodeRange;

void throwException(const char* format, ...);
void throwUnexpectedExpressionException(ExpressionType type, const char* location);

const char* describeLocation(CodeRange code);
