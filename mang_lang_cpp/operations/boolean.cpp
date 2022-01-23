#include "boolean.h"

#include "../factory.h"

bool boolean(Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: return !getEvaluatedDictionary(expression).definitions.empty();
        case NUMBER: return static_cast<bool>(getNumber(expression).value);
        case BOOLEAN: return getBoolean(expression).value;
        case EVALUATED_LIST: return true;
        case EMPTY_LIST: return false;
        case STRING: return true;
        case EMPTY_STRING: return false;
        default: throw UnexpectedExpression(expression.type, "boolean operation");
    }
}
