#include "list.h"

#include "../Expression.h"
#include "../operations/empty.h"
#include "../operations/list.h"

namespace list_functions {

ExpressionPointer prependList(const List& list, ExpressionPointer item) {
    return makeList(new List{list.range, prepend(list.elements, item)});
}

ExpressionPointer prependString(const String& string, ExpressionPointer item) {
    return makeString(new String{string.range, prepend(string.elements, item)});
}

ExpressionPointer prepend(ExpressionPointer in) {
    const auto& elements = ::list(in);
    const auto& item = first(elements);
    const auto& collection = second(elements);
    switch (collection.type) {
        case LIST: return prependList(collection.list(), item);
        case STRING: return prependString(collection.string(), item);
        default: throw std::runtime_error{"Expected list"};
    }
}

ExpressionPointer empty(ExpressionPointer in) {
    return ::empty(in);
}

}
