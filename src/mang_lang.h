#pragma once
#include "mang_lang_string.h"

StringBuilder reformat(const char* code);
StringBuilder evaluate_types(const char* code);
StringBuilder evaluate_all(const char* code);
