#pragma once

#include <string>

#include "../expression.h"

struct BinaryTuple {
    Expression left;
    Expression right;
};

BinaryTuple getDynamicBinaryTuple(Expression in, const char* function);
BinaryTuple getStaticBinaryTuple(Expression in, const char* function);
