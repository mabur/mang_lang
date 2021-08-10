#pragma once
#include "Expression.h"

struct Name;
using NamePointer = std::shared_ptr<const Name>;

struct Name {
    CodeRange range;
    std::string value;
};
