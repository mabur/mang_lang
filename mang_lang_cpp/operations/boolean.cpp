#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"
#include "list.h"

bool boolean(const ExpressionPointer& expression) {
    switch(expression.type) {
        case DICTIONARY: return !expression.dictionary().elements.empty();
        case NUMBER: return expression.number().value;
        case LIST: return !!::list(expression);
        case STRING: return !!list(expression);
        default: throw std::runtime_error{"Expected boolean"};
    }
}
