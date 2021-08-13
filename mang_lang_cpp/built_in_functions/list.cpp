#include "list.h"

#include "../Expression.h"
#include "../operations/empty.h"
#include "../operations/list.h"
#include "../operations/prepend.h"

namespace list_functions {

ExpressionPointer empty(ExpressionPointer in) {
    return ::empty(in);
}

ExpressionPointer prepend(ExpressionPointer in) {
    const auto& elements = ::list(in);
    const auto& item = first(elements);
    const auto& list = second(elements);
    return prepend(list, item);
}

}
