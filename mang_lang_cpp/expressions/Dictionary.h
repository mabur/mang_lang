#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary : public Expression {
    Dictionary(CodeRange range, const Expression* environment)
        : Expression{range, environment, DICTIONARY}
    {}
    std::vector<DictionaryElementPointer> elements;
};
