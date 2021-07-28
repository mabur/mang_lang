#include "prepend.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "list.h"

ExpressionPointer prependList(const ExpressionPointer& list, ExpressionPointer item) {
    auto new_list = ::prepend(::list(list), item);
    return makeList(std::make_shared<List>(list->range, nullptr, new_list));
}

ExpressionPointer prependString(const ExpressionPointer& string, ExpressionPointer item) {
    auto new_list = ::prepend(list(string), item);
    return makeString(std::make_shared<String>(string->range, nullptr, new_list));
}

ExpressionPointer prepend(const ExpressionPointer& expression_smart, ExpressionPointer item) {
    const auto expression = expression_smart.get();
    if (expression->type_ == LIST) {
        return prependList(expression_smart, item);
    }
    if (expression->type_ == STRING) {
        return prependString(expression_smart, item);
    }
    throw std::runtime_error{"Expected list"};
}
