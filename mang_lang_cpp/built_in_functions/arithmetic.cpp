#include "arithmetic.h"

#include <cmath>
#include <limits>

#include "../factory.h"
#include "../container.h"

namespace arithmetic {

double number(Expression expression) {
    return getNumber(expression).value;
}

Expression makeNumber(double x) {
    return makeNumber(new Number(Number{CodeRange{}, x}));
}

double minExpression(double left, Expression right) {
    return std::min(left, number(right));
}

double maxExpression(double left, Expression right) {
    return std::max(left, number(right));
}

double addExpression(double left, Expression right) {
    return left + number(right);
}

double mulExpression(double left, Expression right) {
    return left * number(right);
}

Expression min(Expression in) {

    const auto init = std::numeric_limits<double>::infinity();
    const auto result = new_list::leftFold(init, in, minExpression);
    return makeNumber(result);
}

Expression max(Expression in) {
    const auto init = -std::numeric_limits<double>::infinity();
    const auto result = new_list::leftFold(init, in, maxExpression);
    return makeNumber(result);
}

Expression add(Expression in) {

    const auto init = 0.0;
    const auto result = new_list::leftFold(init, in, addExpression);
    return makeNumber(result);
}

Expression mul(Expression in) {

    const auto init = 1.0;
    const auto result = new_list::leftFold(init, in, mulExpression);
    return makeNumber(result);
}

Expression sub(Expression in) {
    const auto left = new_list::first(in);
    const auto right = new_list::second(in);
    const auto result = number(left) - number(right);
    return makeNumber(result);
}

Expression div(Expression in) {
    const auto left = new_list::first(in);
    const auto right = new_list::second(in);
    const auto result = number(left) / number(right);
    return makeNumber(result);
}

template<typename Predicate>
Expression allOfNeighbours(Expression in, Predicate predicate) {
    auto list = in;
    while (list.type != EMPTY_LIST && new_list::rest(list).type != EMPTY_LIST) {
        const auto left = new_list::first(list);
        const auto right = new_list::second(list);
        if (!predicate(left, right)) {
            return makeNumber(false);
        }
        list = new_list::rest(list);
    }
    return makeNumber(true);
}

bool isLess(Expression left, Expression right) {
    return number(left) < number(right);
}

bool isLeq(Expression left, Expression right) {
    return number(left) <= number(right);
}

Expression less(Expression in) {
    return allOfNeighbours(in, isLess);
}

Expression less_or_equal(Expression in) {
    return allOfNeighbours(in, isLeq);
}

Expression abs(Expression in) {
    return makeNumber(std::fabs(number(in)));
}

Expression sqrt(Expression in) {
    return makeNumber(std::sqrt(number(in)));
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

Expression ascii_number(Expression in) {
    return makeNumber(getCharacter(in).value);
}

Expression ascii_character(Expression in) {
    return makeCharacter(new Character{CodeRange{}, static_cast<char>(getNumber(in).value)});
}

}
