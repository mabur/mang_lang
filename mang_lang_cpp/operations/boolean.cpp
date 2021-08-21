#include "boolean.h"

#include "../factory.h"

bool boolean(ExpressionPointer expression) {
    switch (expression.type) {
        case DICTIONARY: return !getDictionary(expression).elements.empty();
        case NUMBER: return static_cast<bool>(getNumber(expression).value);
        case LIST: return static_cast<bool>(getList(expression).elements);
        case STRING: return static_cast<bool>(getString(expression).elements);
        default: throw std::runtime_error{"Expected boolean"};
    }
}
