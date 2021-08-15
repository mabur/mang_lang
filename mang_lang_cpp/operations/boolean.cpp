#include "boolean.h"

#include "../Expression.h"

bool boolean(ExpressionPointer expression) {
    switch (expression.type) {
        case DICTIONARY: return !dictionary(expression).elements.empty();
        case NUMBER: return static_cast<bool>(number2(expression).value);
        case LIST: return static_cast<bool>(list2(expression).elements);
        case STRING: return static_cast<bool>(string(expression).elements);
        default: throw std::runtime_error{"Expected boolean"};
    }
}
