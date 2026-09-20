#pragma once

#include "../expression.h"

struct BinaryTuple {
    Expression left;
    Expression right;
    Expression error;
    bool ok;
};

BinaryTuple getBinaryTuple(Expression in, const char* function);

// Unpacks `in` as a tuple of two items and applies `function2` to them.
// Returns the unpacking error if `in` is not such a tuple.
Expression applyBinaryTuple(Expression in, const char* function, BinaryFunctionPointer function2);
