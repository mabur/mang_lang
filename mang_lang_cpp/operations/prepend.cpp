#include "prepend.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

ExpressionPointer prependList(const List* list, ExpressionPointer item) {
    auto new_list = ::prepend(list->list(), item);
    return std::make_shared<typename List::value_type>(list->range(), nullptr, new_list);
}

ExpressionPointer prependString(const String* string, ExpressionPointer item) {
    auto new_list = ::prepend(string->list(), item);
    return std::make_shared<String>(string->range(), nullptr, new_list);
}

ExpressionPointer prepend(const Expression* expression, ExpressionPointer item) {
    if (expression->type_ == LIST) {
        return prependList(dynamic_cast<const List *>(expression), item);
    }
    if (expression->type_ == STRING) {
        return prependString(dynamic_cast<const String *>(expression), item);
    }
    throw std::runtime_error{"Expected list"};
}
