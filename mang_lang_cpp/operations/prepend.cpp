#include "prepend.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "list.h"

ExpressionPointer prependList(const ExpressionPointer& list, ExpressionPointer item) {
    auto new_list = ::prepend(::list(list), item);
    return makeList(std::make_shared<List>(list->range, ExpressionPointer{}, new_list));
}

ExpressionPointer prependString(const ExpressionPointer& string, ExpressionPointer item) {
    auto new_list = ::prepend(list(string), item);
    return makeString(std::make_shared<String>(string->range, ExpressionPointer{}, new_list));
}

ExpressionPointer prepend(const ExpressionPointer& expression, ExpressionPointer item) {
    switch (expression.type) {
        case LIST: return prependList(expression, item);
        case STRING: return prependString(expression, item);
        default: throw std::runtime_error{"Expected list"};
    }
}
