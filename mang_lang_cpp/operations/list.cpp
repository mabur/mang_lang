#include "list.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

const InternalList& listList(const List* list) {
    return list->elements;
}

const InternalList& listString(const String* string) {
    return string->elements;
}

const InternalList& list(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == LIST) {
        return listList(dynamic_cast<const List *>(expression));
    }
    if (expression->type_ == STRING) {
        return listString(dynamic_cast<const String *>(expression));
    }
    throw std::runtime_error{"Expected list"};
}
