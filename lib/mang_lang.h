#pragma once
#include "mang_lang_string.h"

StringBuilder reformat(const char* code);
StringBuilder evaluate_types(const char* code);
StringBuilder evaluate_all(const char* code);
// Same as evaluate_all, but also prints what the program allocated.
StringBuilder evaluate_all_with_statistics(const char* code);
