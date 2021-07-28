#include "prepend.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "list.h"

ExpressionPointer prependList(const List* list, ExpressionPointer item) {
    auto new_list = ::prepend(::list(list), item);
    return makeList(std::make_shared<List>(list->range, nullptr, new_list));
}

ExpressionPointer prependString(const String* string, ExpressionPointer item) {
    auto new_list = ::prepend(list(string), item);
    return makeString(std::make_shared<String>(string->range, nullptr, new_list));
}

ExpressionPointer prepend(const ExpressionPointer& expression_smart, ExpressionPointer item) {
    const auto expression = expression_smart.get();
    if (expression->type_ == LIST) {
        return prependList(dynamic_cast<const List *>(expression), item);
    }
    if (expression->type_ == STRING) {
        return prependString(dynamic_cast<const String *>(expression), item);
    }
    throw std::runtime_error{"Expected list"};
}
