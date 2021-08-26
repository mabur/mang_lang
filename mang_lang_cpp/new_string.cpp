#include "new_string.h"

#include "factory.h"

namespace new_string {

ExpressionPointer first(ExpressionPointer list) {
    return getNewString(list).first;
}

ExpressionPointer rest(ExpressionPointer expression) {
    return getNewString(expression).rest;
}

ExpressionPointer second(ExpressionPointer list) {
    return first(rest(list));
}

} // namespace new_string
