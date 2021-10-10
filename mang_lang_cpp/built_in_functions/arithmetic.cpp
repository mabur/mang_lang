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

Expression makeBoolean(bool x) {
    return makeBoolean(new Boolean{CodeRange{}, x});
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
    const auto binary = new_list::getBinaryInput(in);
    const auto result = number(binary.left) - number(binary.right);
    return makeNumber(result);
}

Expression div(Expression in) {
    const auto binary = new_list::getBinaryInput(in);
    const auto result = number(binary.left) / number(binary.right);
    return makeNumber(result);
}

bool less(Expression left, Expression right) {
    return number(left) < number(right);
}

bool lessOrEqual(Expression left, Expression right) {
    return number(left) <= number(right);
}

Expression are_all_less(Expression in) {
    return makeBoolean(new_list::allOfNeighbours(in, less));
}

Expression are_all_less_or_equal(Expression in) {
    return makeBoolean(new_list::allOfNeighbours(in, lessOrEqual));
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
