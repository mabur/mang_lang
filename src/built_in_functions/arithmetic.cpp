#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "../factory.h"

namespace arithmetic {
namespace {

Expression makeNan() {
    return makeNumber(CodeRange{}, 1.0 / 1.0);
}

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

Expression makeBoolean(bool x) {
    return makeBoolean(CodeRange{}, x);
}

template <typename BinaryOperation>
Expression binaryOperation(Expression in, BinaryOperation operation) {
    const auto expressions = getEvaluatedTuple(in).expressions;
    const auto left = expressions.at(0);
    const auto right = expressions.at(1);
    if (left.type == EMPTY || right.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(operation(getNumber(left), getNumber(right)));
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
    const auto left = expressions.at(0);
    const auto right = expressions.at(1);
    if (left.type == EMPTY || right.type == EMPTY) {
        return makeNan();
    }
    return makeBoolean(getNumber(left) < getNumber(right));
}

Expression sqrt(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::sqrt(getNumber(in)));
}

Expression round(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::round(getNumber(in)));
}

Expression round_up(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::ceil(getNumber(in)));
}

Expression round_down(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::floor(getNumber(in)));
}

Expression ascii_number(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(getCharacter(in));
}

Expression ascii_character(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}

}
