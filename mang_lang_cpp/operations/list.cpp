#include "list.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

const InternalList& listList(const List* list) {
    return list->elements;
}

const InternalList& listString(const String* string) {
    return string->elements;
}

const InternalList& list(const Expression* expression) {
    if (expression->type_ == LIST) {
        return listList(dynamic_cast<const List *>(expression));
    }
    if (expression->type_ == STRING) {
        return listString(dynamic_cast<const String *>(expression));
    }
    throw std::runtime_error{"Expected list"};
}

ExpressionPointer emptyList(const List* list) {
    return std::make_shared<List>(list->range(), nullptr, nullptr);
}

ExpressionPointer emptyString(const String* string) {
    return std::make_shared<String>(string->range(), nullptr, nullptr);
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
