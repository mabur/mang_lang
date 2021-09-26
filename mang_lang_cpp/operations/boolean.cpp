#include "boolean.h"

#include "../factory.h"

bool boolean(Expression expression) {
    switch (expression.type) {
        case DICTIONARY: return !getDictionary(expression).statements.empty();
        case NUMBER: return static_cast<bool>(getNumber(expression).value);
        case NEW_LIST: return true;
        case NEW_EMPTY_LIST: return false;
        case STRING: return true;
        case EMPTY_STRING: return false;
        default: throw std::runtime_error{"Expected boolean"};
    }
}
