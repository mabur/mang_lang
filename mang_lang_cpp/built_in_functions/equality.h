#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/is_equal.h"
#include "../operations/list.h"

#include "../factory.h"

namespace equality {

ExpressionPointer makeNumber(const ExpressionPointer& in, double x) {
    return makeNumber(std::make_shared<Number>(in->range, nullptr, x));
}

ExpressionPointer equal(const ExpressionPointer& in) {
    const auto &elements = list(in.get());
    const auto &left = elements->first;
    const auto &right = elements->rest->first;
    const auto value = isEqual(left, right);
    return makeNumber(in, value);
}

ExpressionPointer unequal(const ExpressionPointer& in) {
    const auto &elements = list(in.get());
    const auto &left = elements->first;
    const auto &right = elements->rest->first;
    const auto value = !isEqual(left, right);
    return makeNumber(in, value);
}

}
