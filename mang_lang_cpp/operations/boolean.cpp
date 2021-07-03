#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/Number.h"

bool boolean(const Dictionary& dictionary) {
    return !dictionary.elements.empty();
}

bool boolean(const Number& number) {
    return static_cast<bool>(number.value);
}
