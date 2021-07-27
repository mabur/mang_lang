#pragma once

#include <cmath>
#include <limits>

#include "../expressions/Number.h"

#include "../operations/list.h"
#include "../operations/number.h"

#include "../factory.h"

namespace arithmetic {

namespace {

const Expression* first(const Expression& in) {
    return list(&in)->first.get();
}

const Expression* second(const Expression& in) {
    return list(&in)->rest->first.get();
}

}

ExpressionPointer makeNumber(const Expression& in, double x) {
    return makeNumber(std::make_shared<Number>(in.range, nullptr, x));
}

ExpressionPointer min(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::min(left, number(right.get()));
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(&in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer max(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::max(left, number(right.get()));
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(list(&in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer add(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left + number(right.get());
    };
    const auto init = 0;
    const auto result = leftFold(list(&in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer mul(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left * number(right.get());
    };
    const auto init = 1.0;
    const auto result = leftFold(list(&in), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer sub(const Expression& in) {
    const auto result = number(first(in)) - number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer div(const Expression& in) {
    const auto result = number(first(in)) / number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer abs(const Expression& in) {
    const auto result = std::fabs(number(&in));
    return makeNumber(in, result);
}

ExpressionPointer sqrt(const Expression& in) {
    const auto result = std::sqrt(number(&in));
    return makeNumber(in, result);
}

ExpressionPointer less(const Expression& in) {
    const auto result = number(first(in)) < number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer less_or_equal(const Expression& in) {
    const auto result = number(first(in)) <= number(second(in));
    return makeNumber(in, result);
}

ExpressionPointer round(const Expression& in) {
    return makeNumber(in, std::round(number(&in)));
}

ExpressionPointer round_up(const Expression& in) {
    return makeNumber(in, std::ceil(number(&in)));
}

ExpressionPointer round_down(const Expression& in) {
    return makeNumber(in, std::floor(number(&in)));
}

}
