#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary : public Expression {
    Dictionary(
        CodeRange range,
        ExpressionPointer environment,
        DictionaryElements elements
    ) : Expression{range, environment, DICTIONARY}, elements{std::move(elements)}
    {}
    DictionaryElements elements;
};
