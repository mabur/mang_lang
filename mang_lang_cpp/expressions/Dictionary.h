#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary : public Expression {
    Dictionary(
        CodeRange range,
        ExpressionPointer environment,
        std::vector<DictionaryElementPointer> elements
    ) : Expression{range, environment, DICTIONARY}, elements{std::move(elements)}
    {}
    std::vector<DictionaryElementPointer> elements;
};
