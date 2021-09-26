#include "list.h"

#include "../factory.h"
#include "../new_string.h"

namespace list_functions {

Expression empty(Expression in) {
    switch (in.type) {
        case NEW_LIST: return makeNewEmptyList(new NewEmptyList{});
        case NEW_EMPTY_LIST: return makeNewEmptyList(new NewEmptyList{});
        case STRING: return makeEmptyString(new EmptyString{});
        case EMPTY_STRING: return makeEmptyString(new EmptyString{});
        default: throw std::runtime_error{"Expected list"};
    }
}

Expression prepend(Expression in) {
    const auto& item = new_list::first(in);
    const auto& collection = new_list::second(in);
    switch (collection.type) {
        case NEW_LIST: return new_list::prepend(collection, item);
        case NEW_EMPTY_LIST: return new_list::prepend(collection, item);
        case STRING: return new_string::prepend(collection, item);
        case EMPTY_STRING: return new_string::prepend(collection, item);
        default: throw std::runtime_error{"Expected list"};
    }
}

}
