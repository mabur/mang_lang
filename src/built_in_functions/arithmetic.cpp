#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "../factory.h"

namespace arithmetic {
namespace {

double number(Expression expression) {
    switch (expression.type) {
        case NUMBER : return getNumber(expression);
        //case ANY : return 0.0 / 0.0;
        default: throw StaticTypeError(expression.type, "built-in number");
    }
}

double character(Expression expression) {
    switch (expression.type) {
        case CHARACTER : return getCharacter(expression);
        //case ANY : return '\0';
        default: throw StaticTypeError(expression.type, "built-in character");
    }
}

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
    return makeNumber(operation(number(left), number(right)));
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
    return makeBoolean(number(left) < number(right));
}

Expression sqrt(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::sqrt(number(in)));
}

Expression round(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::round(number(in)));
}

Expression round_up(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::ceil(number(in)));
}

Expression round_down(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(std::floor(number(in)));
}

Expression ascii_number(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeNumber(character(in));
}

Expression ascii_character(Expression in) {
    if (in.type == EMPTY) {
        return makeNan();
    }
    return makeCharacter(CodeRange{}, static_cast<char>(number(in)));
}

}
