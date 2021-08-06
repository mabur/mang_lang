#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

bool boolean(const ExpressionPointer& expression) {
    switch (expression.type) {
        case DICTIONARY: return !expression.dictionary().elements.empty();
        case NUMBER: return static_cast<bool>(expression.number().value);
        case LIST: return static_cast<bool>(list(expression));
        case STRING: return static_cast<bool>(list(expression));
        default: throw std::runtime_error{"Expected boolean"};
    }
}
