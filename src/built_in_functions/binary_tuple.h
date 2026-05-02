#pragma once

#include "../expression.h"

struct BinaryTuple {
    Expression left;
    Expression right;
};

BinaryTuple getBinaryTuple(Expression in, const char* function);
BinaryTuple getBinaryTuple(Expression in, const char* function);
