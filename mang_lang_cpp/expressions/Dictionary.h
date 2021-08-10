#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary {
    CodeRange range;
    ExpressionPointer environment;
    DictionaryElements elements;
};
