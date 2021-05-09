#pragma once

#include "../expressions/Expression.h"
#include "../expressions/List.h"

namespace list_functions {

ExpressionPointer first(const Expression& in) {
    return in.first();
}

ExpressionPointer rest(const Expression& in) {
    return in.rest();
}

ExpressionPointer reverse(const Expression& in) {
    return in.reverse();
}

ExpressionPointer prepend(const Expression& in) {
    auto item = in.list()->first;
    auto list = in.list()->rest->first;
    return list->prepend(item);
}

}
