#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "../factory.h"
#include "../container.h"

namespace arithmetic {
namespace {

double number(Expression expression) {
    return getNumber(expression).value;
}

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, Number{x});
}

Expression makeBoolean(bool x) {
    return makeBoolean(CodeRange{}, Boolean{x});
}

template <typename BinaryOperation>
Expression binaryOperation(Expression in, BinaryOperation operation) {
    const auto expressions = getEvaluatedTuple(in).expressions;
    const auto left = number(expressions.at(0));
    const auto right = number(expressions.at(1));
    return makeNumber(operation(left, right));
}

} // namespace

Expression add(Expression in) {
    return binaryOperation(in, std::plus<>());
}

Expression mul(Expression in) {
    return binaryOperation(in, std::multiplies<>());
}

Expression sub(Expression in) {
    return binaryOperation(in, std::minus<>());
}

Expression div(Expression in) {
    return binaryOperation(in, std::divides<>());
}

bool less(Expression left, Expression right) {
    return number(left) < number(right);
}

bool lessOrEqual(Expression left, Expression right) {
    return number(left) <= number(right);
}

Expression are_all_less(Expression in) {
    return makeBoolean(allOfNeighbours(in, less, EMPTY_STACK, getEvaluatedStack));
}

Expression are_all_less_or_equal(Expression in) {
    return makeBoolean(
        allOfNeighbours(in, lessOrEqual, EMPTY_STACK, getEvaluatedStack));
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
    return makeCharacter(CodeRange{}, Character{static_cast<char>(getNumber(in).value)});
}

}
