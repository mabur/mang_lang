#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/is_equal.h"
#include "../operations/list.h"

namespace equality {

ExpressionPointer makeNumber(const Expression &in, double x) {
    return std::make_shared<Number>(in.range(), nullptr, x);
}

ExpressionPointer equal(const Expression &in) {
    const auto &elements = list(&in);
    const auto &left = elements->first;
    const auto &right = elements->rest->first;
    const auto value = isEqual(left.get(), right.get());
    return makeNumber(in, value);
}

ExpressionPointer unequal(const Expression &in) {
    const auto &elements = list(&in);
    const auto &left = elements->first;
    const auto &right = elements->rest->first;
    const auto value = !isEqual(left.get(), right.get());
    return makeNumber(in, value);
}

}
