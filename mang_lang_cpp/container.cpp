#include "container.h"

#include "factory.h"

namespace new_list {

Expression first(Expression list) {
    return getList(list).first;
}

Expression rest(Expression expression) {
    return getList(expression).rest;
}

Expression second(Expression list) {
    return first(rest(list));
}

} // namespace new_list
