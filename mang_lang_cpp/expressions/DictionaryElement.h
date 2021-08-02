#pragma once

#include <memory>
#include <vector>
#include "Expression.h"
#include "Name.h"

struct DictionaryElement;

using DictionaryElementPointer = std::shared_ptr<DictionaryElement>;
using DictionaryElements = std::vector<DictionaryElementPointer>;

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        ExpressionPointer environment,
        ExpressionType type,
        NamePointer name,
        ExpressionPointer expression,
        size_t jump_true,
        size_t jump_false,
        size_t dictionary_index
    );
    NamePointer name;
    ExpressionPointer expression;
    size_t jump_true_;
    size_t jump_false_;
    size_t dictionary_index_;
};

void setContext(DictionaryElements& elements);
