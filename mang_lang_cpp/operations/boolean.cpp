#include "boolean.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"
#include "list.h"

bool booleanDictionary(const Dictionary* dictionary) {
    return !dictionary->elements.empty();
}

bool booleanNumber(const Number* number) {
    return static_cast<bool>(number->value);
}

bool boolean(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == DICTIONARY) {
        return booleanDictionary(dynamic_cast<const Dictionary *>(expression));
    }
    if (type == NUMBER) {
        return booleanNumber(dynamic_cast<const Number *>(expression));
    }
    if (type == LIST) {
        return !!::list(expression_smart);
    }
    if (type == STRING) {
        return !!list(expression_smart);
    }
    throw std::runtime_error{"Expected boolean"};
}
