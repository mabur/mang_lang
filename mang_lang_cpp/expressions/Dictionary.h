#pragma once
#include "DictionaryElement.h"
#include "Expression.h"

struct Dictionary {
    CodeRange range;
    ExpressionPointer environment;
    DictionaryElements elements;
};
