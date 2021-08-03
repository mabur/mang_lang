#pragma once

#include <memory>
#include <vector>
#include "Expression.h"
#include "Name.h"

struct DictionaryElement;

struct DictionaryElementPointer {
    std::shared_ptr<const DictionaryElement> inner;
};

//using DictionaryElementPointer = std::shared_ptr<const DictionaryElement>;
using DictionaryElements = std::vector<DictionaryElementPointer>;

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        ExpressionPointer environment,
        ExpressionType type,
        NamePointer name,
        ExpressionPointer expression,
        size_t while_index,
        size_t end_index,
        size_t name_index
    );
    NamePointer name;
    ExpressionPointer expression;
    size_t while_index_;
    size_t end_index_;
    size_t name_index_;
};

DictionaryElements setContext(const DictionaryElements& elements);
