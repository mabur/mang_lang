#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

bool booleanDictionary(const Dictionary& dictionary) {
    return !dictionary.elements.empty();
}

bool booleanNumber(const Number& number) {
    return static_cast<bool>(number.value);
}

bool booleanList(const List& list) {
    return !!list.list();
}

bool booleanString(const String& string) {
    return !!string.list();
}
