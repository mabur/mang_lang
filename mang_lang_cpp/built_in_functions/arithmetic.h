#pragma once

#include <cmath>
#include <limits>

#include "../expressions/Number.h"

#include "../operations/list.h"
#include "../operations/number.h"

#include "../factory.h"

namespace arithmetic {

ExpressionPointer makeNumber(double x) {
    return makeNumber(new Number(Number{CodeRange{}, x}));
}

ExpressionPointer min(ExpressionPointer in) {
    const auto operation = [](double left, ExpressionPointer right) -> double {
        return std::min(left, number(right));
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer max(ExpressionPointer in) {
    const auto operation = [](double left, ExpressionPointer right) -> double {
        return std::max(left, number(right));
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer add(ExpressionPointer in) {
    const auto operation = [](double left, ExpressionPointer right) -> double {
        return left + number(right);
    };
    const auto init = 0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer mul(ExpressionPointer in) {
    const auto operation = [](double left, ExpressionPointer right) -> double {
        return left * number(right);
    };
    const auto init = 1.0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer sub(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) - number(right);
    return makeNumber(result);
}

ExpressionPointer div(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) / number(right);
    return makeNumber(result);
}

ExpressionPointer abs(ExpressionPointer in) {
    const auto result = std::fabs(number(in));
    return makeNumber(result);
}

ExpressionPointer sqrt(ExpressionPointer in) {
    const auto result = std::sqrt(number(in));
    return makeNumber(result);
}

ExpressionPointer less(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) < number(right);
    return makeNumber(result);
}

ExpressionPointer less_or_equal(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) <= number(right);
    return makeNumber(result);
}

ExpressionPointer round(ExpressionPointer in) {
    return makeNumber(std::round(number(in)));
}

ExpressionPointer round_up(ExpressionPointer in) {
    return makeNumber(std::ceil(number(in)));
}

ExpressionPointer round_down(ExpressionPointer in) {
    return makeNumber(std::floor(number(in)));
}

}
