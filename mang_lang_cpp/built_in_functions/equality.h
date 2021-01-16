#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

namespace equality {

ExpressionPointer makeNumber(const Expression &in, double x) {
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, x);
}

ExpressionPointer equal(const Expression &in) {
    const auto &elements = in.list();
    const auto &left = elements.at(0);
    const auto &right = elements.at(1);
    const auto value = left->isEqual(right.get());
    return makeNumber(in, value);
}

ExpressionPointer unequal(const Expression &in) {
    const auto &elements = in.list();
    const auto &left = elements.at(0);
    const auto &right = elements.at(1);
    const auto value = !left->isEqual(right.get());
    return makeNumber(in, value);
}

}
