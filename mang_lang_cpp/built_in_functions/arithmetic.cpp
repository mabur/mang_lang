#include "arithmetic.h"

#include <cmath>

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

} // namespace

Expression add(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto result = number(binary.left) + number(binary.right);
    return makeNumber(result);
}

Expression mul(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto result = number(binary.left) * number(binary.right);
    return makeNumber(result);
}

Expression sub(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto result = number(binary.left) - number(binary.right);
    return makeNumber(result);
}

Expression div(Expression in) {
    const auto binary = getBinaryInput(in);
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
