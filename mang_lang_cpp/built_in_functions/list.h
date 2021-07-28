#pragma once

#include "../expressions/Expression.h"
#include "../expressions/List.h"
#include "../operations/empty.h"
#include "../operations/list.h"
#include "../operations/prepend.h"

namespace list_functions {

ExpressionPointer empty(const ExpressionPointer& in) {
    return ::empty(in);
}

ExpressionPointer prepend(const ExpressionPointer& in) {
    auto item = ::list(in.get())->first;
    auto list = ::list(in.get())->rest->first;
    return prepend(list, item);
}

}
