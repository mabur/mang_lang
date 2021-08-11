#include "prepend.h"

ExpressionPointer prependList(const List& list, ExpressionPointer item) {
    auto new_list = ::prepend(list.elements, item);
    return makeList(new List{list.range, new_list});
}

ExpressionPointer prependString(const String& string, ExpressionPointer item) {
    auto new_list = ::prepend(string.elements, item);
    return makeString(new String{string.range, new_list});
}

ExpressionPointer prepend(ExpressionPointer expression, ExpressionPointer item) {
    switch (expression.type) {
        case LIST: return prependList(expression.list(), item);
        case STRING: return prependString(expression.string(), item);
        default: throw std::runtime_error{"Expected list"};
    }
}
