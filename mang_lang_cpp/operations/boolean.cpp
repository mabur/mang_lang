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

bool booleanList(const List* list) {
    return !!::list(list);
}

bool booleanString(const String* string) {
    return !!list(string);
}

bool boolean(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == DICTIONARY) {
        return booleanDictionary(dynamic_cast<const Dictionary *>(expression));
    }
    if (expression->type_ == NUMBER) {
        return booleanNumber(dynamic_cast<const Number *>(expression));
    }
    if (expression->type_ == LIST) {
        return booleanList(dynamic_cast<const List *>(expression));
    }
    if (expression->type_ == STRING) {
        return booleanString(dynamic_cast<const String *>(expression));
    }
    throw std::runtime_error{"Expected boolean"};
}
