#include "list.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

const InternalList& listListBase(const List& list) {
    return list.elements;
}

const InternalList& listListBase(const String& list) {
    return list.elements;
}

ExpressionPointer emptyListBase(const List& list) {
    return std::make_shared<List>(list.range(), nullptr, nullptr);
}

ExpressionPointer emptyListBase(const String& list) {
    return std::make_shared<String>(list.range(), nullptr, nullptr);
}

ExpressionPointer prependListBase(const List& list, ExpressionPointer item) {
    auto new_list = ::prepend(list.list(), item);
    return std::make_shared<typename List::value_type>(list.range(), nullptr, new_list);
}

ExpressionPointer prependListBase(const String& list, ExpressionPointer item) {
    auto new_list = ::prepend(list.list(), item);
    return std::make_shared<String>(list.range(), nullptr, new_list);
}
