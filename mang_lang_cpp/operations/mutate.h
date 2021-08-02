#pragma once

#include "../expressions/DictionaryElement.h"

DictionaryElements mutate(
    const DictionaryElementPointer& dictionary_element,
    const ExpressionPointer& environment,
    std::ostream& log,
    const DictionaryElements& elements);
