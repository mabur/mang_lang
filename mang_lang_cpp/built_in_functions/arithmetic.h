#pragma once

#include <cmath>
#include <limits>

#include "../expressions/Number.h"

#include "../operations/list.h"
#include "../operations/number.h"

#include "../factory.h"

namespace arithmetic {

ExpressionPointer makeNumber(double x) {
    return makeNumber(std::make_shared<Number>(CodeRange{}, ExpressionPointer{}, x));
}

ExpressionPointer min(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::min(left, number(right));
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer max(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::max(left, number(right));
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer add(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left + number(right);
    };
    const auto init = 0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer mul(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left * number(right);
    };
    const auto init = 1.0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(result);
}

ExpressionPointer sub(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) - number(right);
    return makeNumber(result);
}

ExpressionPointer div(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) / number(right);
    return makeNumber(result);
}

ExpressionPointer abs(const ExpressionPointer& in) {
    const auto result = std::fabs(number(in));
    return makeNumber(result);
}

ExpressionPointer sqrt(const ExpressionPointer& in) {
    const auto result = std::sqrt(number(in));
    return makeNumber(result);
}

ExpressionPointer less(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) < number(right);
    return makeNumber(result);
}

ExpressionPointer less_or_equal(const ExpressionPointer& in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) <= number(right);
    return makeNumber(result);
}

ExpressionPointer round(const ExpressionPointer& in) {
    return makeNumber(std::round(number(in)));
}

ExpressionPointer round_up(const ExpressionPointer& in) {
    return makeNumber(std::ceil(number(in)));
}

ExpressionPointer round_down(const ExpressionPointer& in) {
    return makeNumber(std::floor(number(in)));
}

}
