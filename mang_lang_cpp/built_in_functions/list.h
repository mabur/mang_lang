#pragma once

#include "../expressions/Expression.h"
#include "../expressions/List.h"

namespace list_functions {

ExpressionPointer empty(const Expression& in) {
    return in.empty();
}

ExpressionPointer first(const Expression& in) {
    return in.first();
}

ExpressionPointer rest(const Expression& in) {
    return in.rest();
}

ExpressionPointer prepend(const Expression& in) {
    auto item = in.list()->first;
    auto list = in.list()->rest->first;
    return list->prepend(item);
}

}
