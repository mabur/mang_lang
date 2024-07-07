#pragma once
#include <string>
#include "string.h"

DynamicString reformat(const std::string& code);
DynamicString evaluate_types(const std::string& code);
DynamicString evaluate_all(const std::string& code);
