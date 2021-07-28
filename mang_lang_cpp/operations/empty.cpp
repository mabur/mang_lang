#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer emptyList(const List* list) {
    return makeList(std::make_shared<List>(list->range, nullptr, nullptr));
}

ExpressionPointer emptyString(const String* string) {
    return makeString(std::make_shared<String>(string->range, nullptr, nullptr));
}

ExpressionPointer empty(const Expression* expression) {
    if (expression->type_ == LIST) {
        return emptyList(dynamic_cast<const List *>(expression));
    }
    if (expression->type_ == STRING) {
        return emptyString(dynamic_cast<const String *>(expression));
    }
    throw std::runtime_error{"Expected list"};
}
