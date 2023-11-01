#pragma once

#include <string>

#include "../expression.h"

struct BinaryTuple {
    Expression left;
    Expression right;
};

BinaryTuple getDynamicBinaryTuple(Expression in, const std::string& function);
BinaryTuple getStaticBinaryTuple(Expression in, const std::string& function);
