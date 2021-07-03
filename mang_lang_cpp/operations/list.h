#pragma once

#include "../expressions/Expression.h"

template<typename List>
auto& listListBase(const List& list) {
    return list.elements;
}

template<typename List>
ExpressionPointer emptyListBase(const List& list) {
    return std::make_shared<typename List::value_type>(list.range(), nullptr, nullptr);
}

template<typename List>
ExpressionPointer prependListBase(const List& list, ExpressionPointer item) {
    auto new_list = ::prepend(list.list(), item);
    return std::make_shared<typename List::value_type>(list.range(), nullptr, new_list);
}
