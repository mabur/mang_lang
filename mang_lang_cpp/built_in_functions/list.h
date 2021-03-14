#pragma once

#include "../expressions/Expression.h"
#include "../expressions/List.h"

namespace list_functions {

ExpressionPointer first(const Expression& in) {
    return in.list()->first;
}

ExpressionPointer rest(const Expression& in) {
    return std::make_shared<List>(in.begin(), in.end(), nullptr, in.list()->rest);
}

ExpressionPointer prepend(const Expression& in) {
    auto list = in.lookup("list")->list();
    auto item = in.lookup("item");
    auto new_list = prepend(list, item);
    return std::make_shared<List>(in.begin(), in.end(), nullptr, new_list);
}

}
