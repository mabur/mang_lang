#pragma once
#include <string>
#include "string.h"

DynamicString reformat(const char* code);
DynamicString evaluate_types(const char* code);
DynamicString evaluate_all(const char* code);
