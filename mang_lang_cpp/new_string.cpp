#include "new_string.h"

#include "factory.h"

namespace new_string {

Expression first(Expression list) {
    return getNewString(list).first;
}

Expression rest(Expression expression) {
    return getNewString(expression).rest;
}

Expression second(Expression list) {
    return first(rest(list));
}

} // namespace new_string
