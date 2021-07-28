#pragma once

#include <cmath>
#include <limits>

#include "../expressions/Number.h"

#include "../operations/list.h"
#include "../operations/number.h"

#include "../factory.h"

namespace arithmetic {

namespace {

ExpressionPointer first(const ExpressionPointer& in) {
    return list(in)->first;
}

ExpressionPointer second(const ExpressionPointer& in) {
    return list(in)->rest->first;
}

}

ExpressionPointer makeNumber(const ExpressionPointer& in, double x) {
    return makeNumber(std::make_shared<Number>(in->range, nullptr, x));
}

ExpressionPointer min(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::min(left, number(right));
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer max(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::max(left, number(right));
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer add(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left + number(right);
    };
    const auto init = 0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer mul(const ExpressionPointer& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left * number(right);
    };
    const auto init = 1.0;
    const auto result = leftFold(list(in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer sub(const ExpressionPointer& in) {
    const auto result = number(first(in)) - number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer div(const ExpressionPointer& in) {
    const auto result = number(first(in)) / number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer abs(const ExpressionPointer& in) {
    const auto result = std::fabs(number(in));
    return makeNumber(in, result);
}

ExpressionPointer sqrt(const ExpressionPointer& in) {
    const auto result = std::sqrt(number(in));
    return makeNumber(in, result);
}

ExpressionPointer less(const ExpressionPointer& in) {
    const auto result = number(first(in)) < number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer less_or_equal(const ExpressionPointer& in) {
    const auto result = number(first(in)) <= number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer round(const ExpressionPointer& in) {
    return makeNumber(in, std::round(number(in)));
}

ExpressionPointer round_up(const ExpressionPointer& in) {
    return makeNumber(in, std::ceil(number(in)));
}

ExpressionPointer round_down(const ExpressionPointer& in) {
    return makeNumber(in, std::floor(number(in)));
}

}
