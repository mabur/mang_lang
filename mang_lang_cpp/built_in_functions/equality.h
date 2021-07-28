#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/is_equal.h"
#include "../operations/list.h"

#include "../factory.h"

namespace equality {

ExpressionPointer makeNumber(double x) {
    return makeNumber(std::make_shared<Number>(CodeRange{}, nullptr, x));
}

ExpressionPointer equal(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = isEqual(left, right);
    return makeNumber(value);
}

ExpressionPointer unequal(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = !isEqual(left, right);
    return makeNumber(value);
}

}
