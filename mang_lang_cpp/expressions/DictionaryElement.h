#pragma once

#include <memory>
#include <vector>
#include "Expression.h"
#include "Name.h"

using DictionaryElements = std::vector<ExpressionPointer>;

struct DictionaryElement {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer name;
    ExpressionPointer expression;
    size_t while_index_;
    size_t end_index_;
    size_t name_index_;
};

DictionaryElements setContext(const DictionaryElements& elements);
