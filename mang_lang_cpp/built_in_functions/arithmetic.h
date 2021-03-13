#pragma once

#include <cmath>
#include <limits>

#include "../expressions/Number.h"
#include "../algorithm.h"

namespace arithmetic {

ExpressionPointer makeNumber(const Expression& in, double x) {
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, x);
}

ExpressionPointer min(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::min(left, right->number());
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(in.list(), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer max(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return std::max(left, right->number());
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(in.list(), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer add(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left + right->number();
    };
    const auto init = 0;
    const auto result = leftFold(in.list(), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer mul(const Expression& in) {
    const auto operation = [](double left, const ExpressionPointer& right) -> double {
        return left * right->number();
    };
    const auto init = 1.0;
    const auto result = leftFold(in.list(), init, operation);
    return makeNumber(in, result);
}

ExpressionPointer sub(const Expression& in) {
    const auto result = in.list().at(0)->number() - in.list().at(1)->number();
    return makeNumber(in, result);
}

ExpressionPointer div(const Expression& in) {
    const auto result = in.list().at(0)->number() / in.list().at(1)->number();
    return makeNumber(in, result);
}

ExpressionPointer abs(const Expression& in) {
    const auto result = std::fabs(in.number());
    return makeNumber(in, result);
}

ExpressionPointer sqrt(const Expression& in) {
    const auto result = std::sqrt(in.number());
    return makeNumber(in, result);
}

}
