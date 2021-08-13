#include "boolean.h"

bool boolean(ExpressionPointer expression) {
    switch (expression.type) {
        case DICTIONARY: return !expression.dictionary().elements.empty();
        case NUMBER: return static_cast<bool>(expression.number().value);
        case LIST: return static_cast<bool>(expression.list().elements);
        case STRING: return static_cast<bool>(expression.string().elements);
        default: throw std::runtime_error{"Expected boolean"};
    }
}
