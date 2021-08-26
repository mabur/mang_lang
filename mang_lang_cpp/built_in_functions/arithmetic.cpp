#include "arithmetic.h"

#include <cmath>
#include <limits>

#include "../factory.h"

namespace arithmetic {

double number(Expression expression) {
    return getNumber(expression).value;
}

Expression makeNumber(double x) {
    return makeNumber(new Number(Number{CodeRange{}, x}));
}

Expression min(Expression in) {
    const auto operation = [](double left, Expression right) -> double {
        return std::min(left, number(right));
    };
    const auto init = std::numeric_limits<double>::infinity();
    const auto result = leftFold(getList(in).elements, init, operation);
    return makeNumber(result);
}

Expression max(Expression in) {
    const auto operation = [](double left, Expression right) -> double {
        return std::max(left, number(right));
    };
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = leftFold(getList(in).elements, init, operation);
    return makeNumber(result);
}

Expression add(Expression in) {
    const auto operation = [](double left, Expression right) -> double {
        return left + number(right);
    };
    const auto init = 0;
    const auto result = leftFold(getList(in).elements, init, operation);
    return makeNumber(result);
}

Expression mul(Expression in) {
    const auto operation = [](double left, Expression right) -> double {
        return left * number(right);
    };
    const auto init = 1.0;
    const auto result = leftFold(getList(in).elements, init, operation);
    return makeNumber(result);
}

Expression sub(Expression in) {
    const auto& elements = getList(in).elements;
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) - number(right);
    return makeNumber(result);
}

Expression div(Expression in) {
    const auto& elements = getList(in).elements;
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) / number(right);
    return makeNumber(result);
}

Expression abs(Expression in) {
    const auto result = std::fabs(number(in));
    return makeNumber(result);
}

Expression sqrt(Expression in) {
    const auto result = std::sqrt(number(in));
    return makeNumber(result);
}

Expression less(Expression in) {
    const auto& elements = getList(in).elements;
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) < number(right);
    return makeNumber(result);
}

Expression less_or_equal(Expression in) {
    const auto& elements = getList(in).elements;
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto result = number(left) <= number(right);
    return makeNumber(result);
}

Expression round(Expression in) {
    return makeNumber(std::round(number(in)));
}

Expression round_up(Expression in) {
    return makeNumber(std::ceil(number(in)));
}

Expression round_down(Expression in) {
    return makeNumber(std::floor(number(in)));
}

}
