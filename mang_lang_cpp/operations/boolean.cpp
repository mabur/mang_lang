#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

bool boolean(const Dictionary& dictionary) {
    return !dictionary.elements.empty();
}

bool boolean(const Number& number) {
    return static_cast<bool>(number.value);
}

bool boolean(const List& list) {
    return !!list.list();
}

bool boolean(const String& string) {
    return !!string.list();
}
