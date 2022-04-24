#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "../factory.h"

namespace arithmetic {
namespace {

double number(Expression expression) {
    switch (expression.type) {
        case NUMBER : return getNumber(expression).value;
        case EMPTY : return 0.0 / 0.0;
        default: throw StaticTypeError(expression.type, "built-in number");
    }
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

Expression less(Expression in) {
    const auto expressions = getEvaluatedTuple(in).expressions;
    const auto left = number(expressions.at(0));
    const auto right = number(expressions.at(1));
    return makeBoolean(left < right);
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
