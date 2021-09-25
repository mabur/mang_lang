#include "list.h"

#include "../factory.h"
#include "../new_string.h"

namespace list_functions {

Expression empty(Expression in) {
    switch (in.type) {
        case LIST: return makeList(new List{});
        case STRING: return makeEmptyString(new EmptyString{});
        case EMPTY_STRING: return makeEmptyString(new EmptyString{});
        default: throw std::runtime_error{"Expected list"};
    }
}

Expression prependList(const List& list, Expression item) {
    return makeList(new List{list.range, prepend(list.elements, item)});
}

Expression prepend(Expression in) {
    const auto& elements = getList(in).elements;
    const auto& item = first(elements);
    const auto& collection = second(elements);
    switch (collection.type) {
        case LIST: return prependList(getList(collection), item);
        case STRING: return new_string::prepend(collection, item);
        case EMPTY_STRING: return new_string::prepend(collection, item);
        default: throw std::runtime_error{"Expected list"};
    }
}

}
