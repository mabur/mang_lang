#pragma once

#include "../expressions/DictionaryElement.h"

size_t jump(
    const DictionaryElementPointer& dictionary_element,
    const ExpressionPointer& environment,
    std::ostream& log);
