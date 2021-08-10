#pragma once
#include "Expression.h"

struct Name;
using NamePointer = std::shared_ptr<const Name>;

struct Name {
    CodeRange range;
    ExpressionPointer environment;
    std::string value;
};
