#include "list.h"

#include "../factory.h"
#include "../container.h"

namespace list_functions {

Expression empty(Expression in) {
    switch (in.type) {
        case LIST: return makeEmptyList(new EmptyList{});
        case EMPTY_LIST: return makeEmptyList(new EmptyList{});
        case STRING: return makeEmptyString(new EmptyString{});
        case EMPTY_STRING: return makeEmptyString(new EmptyString{});
        default: throw std::runtime_error{"Expected list"};
    }
}

Expression prepend(Expression in) {
    const auto binary = new_list::getBinaryInput(in);
    const auto item = binary.left;
    const auto collection = binary.right;
    switch (collection.type) {
        case LIST: return new_list::prepend(collection, item);
        case EMPTY_LIST: return new_list::prepend(collection, item);
        case STRING: return new_string::prepend(collection, item);
        case EMPTY_STRING: return new_string::prepend(collection, item);
        default: throw std::runtime_error{"Expected list"};
    }
}

}
