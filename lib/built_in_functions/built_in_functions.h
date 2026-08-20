#pragma once

#include "../expression.h"

// The fixed, closed set of built-in functions. Shadowing one of these names
// is not a supported program, so both the environment registration below
// and parsing's fast dispatch (see findBuiltIn) can treat this list as the
// single source of truth for what counts as a built-in.
struct BuiltInEntry {
    const char* name;
    FunctionPointer function;       // Used by evaluate().
    FunctionPointer function_types; // Used by evaluate_types().
};

extern const BuiltInEntry BUILT_IN_ENTRIES[];
extern const size_t BUILT_IN_ENTRIES_COUNT;

const BuiltInEntry* findBuiltIn(size_t name_index);

Expression builtIns();
Expression builtInsTypes();
