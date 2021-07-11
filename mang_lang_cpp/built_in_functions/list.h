#pragma once

#include "../expressions/Expression.h"
#include "../expressions/List.h"
#include "../operations/prepend.h"

namespace list_functions {

ExpressionPointer empty(const Expression& in) {
    return in.empty();
}

ExpressionPointer prepend(const Expression& in) {
    auto item = in.list()->first;
    auto list = in.list()->rest->first;
    return prepend(list.get(), item);
}

}
