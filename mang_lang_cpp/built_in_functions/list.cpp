#include "list.h"

#include "../Expression.h"

namespace list_functions {

ExpressionPointer empty(ExpressionPointer in) {
    switch (in.type) {
        case LIST: return makeList(new List{});
        case STRING: return makeString(new String{});
        default: throw std::runtime_error{"Expected list"};
    }
}

ExpressionPointer prependList(const List& list, ExpressionPointer item) {
    return makeList(new List{list.range, prepend(list.elements, item)});
}

ExpressionPointer prependString(const String& string, ExpressionPointer item) {
    return makeString(new String{string.range, prepend(string.elements, item)});
}

ExpressionPointer prepend(ExpressionPointer in) {
    const auto& elements = in.list().elements;
    const auto& item = first(elements);
    const auto& collection = second(elements);
    switch (collection.type) {
        case LIST: return prependList(collection.list(), item);
        case STRING: return prependString(collection.string(), item);
        default: throw std::runtime_error{"Expected list"};
    }
}

}
