#include "list.h"

#include "../factory.h"
#include "../new_string.h"

namespace list_functions {

ExpressionPointer empty(ExpressionPointer in) {
    switch (in.type) {
        case LIST: return makeList(new List{});
        case NEW_STRING: return makeNewEmptyString(new NewEmptyString{});
        case NEW_EMPTY_STRING: return makeNewEmptyString(new NewEmptyString{});
        default: throw std::runtime_error{"Expected list"};
    }
}

ExpressionPointer prependList(const List& list, ExpressionPointer item) {
    return makeList(new List{list.range, prepend(list.elements, item)});
}

ExpressionPointer prepend(ExpressionPointer in) {
    const auto& elements = getList(in).elements;
    const auto& item = first(elements);
    const auto& collection = second(elements);
    switch (collection.type) {
        case LIST: return prependList(getList(collection), item);
        case NEW_STRING: return new_string::prepend(collection, item);
        case NEW_EMPTY_STRING: return new_string::prepend(collection, item);
        default: throw std::runtime_error{"Expected list"};
    }
}

}
