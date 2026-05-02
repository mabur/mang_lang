#include "arithmetic.h"

#include <math.h>

#include "binary_tuple.h"
#include "../factory.h"
#include "../string.h"
#include "../type_check.h"

namespace arithmetic {
namespace {

TypeCheck checkTypeUnaryFunction(Expression in, ExpressionType expected, const char* function) {
    auto result = MAKE(TypeCheck, .ok=true);
    if (in.type != ANY && in.type != expected) {
        result.ok = false;
        result.error = makeEvaluateError({}, format_cstring(
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a %s,\n"
            "but now got %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(in.type)
        ));
    }
    return result;
}

BinaryTuple checkTypeBinaryFunction(Expression in, ExpressionType expected, const char* function) {
    auto result = getBinaryTuple(in, function);
    if (!result.ok) {
        return result;
    }
    if (result.left.type != ANY && result.left.type != expected) {
        result.error = makeEvaluateError({}, format_cstring(
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the first item in the tuple is %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(result.left.type)
        ));
        return result;
    }
    if (result.right.type != ANY && result.right.type != expected) {
        result.error = makeEvaluateError({}, format_cstring(
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the second item in the tuple is %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(result.right.type)
        ));
        return result;
    }
    result.ok = true;
    return result;
}

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

} // namespace

Expression add(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "add");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(type_check.left) + getNumber(type_check.right));
}

Expression mul(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "mul");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(type_check.left) * getNumber(type_check.right));
}

Expression sub(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "sub");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(type_check.left) - getNumber(type_check.right));
}

Expression div(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "div");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(type_check.left) / getNumber(type_check.right));
}

Expression mod(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "mod");
    if (!type_check.ok) return type_check.error;
    return makeNumber(fmod(getNumber(type_check.left), getNumber(type_check.right)));
}

Expression less(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "less");
    if (!type_check.ok) return type_check.error;
    const auto left = getNumber(type_check.left);
    const auto right = getNumber(type_check.right);
    return left < right ?
        Expression{0, CodeRange{}, YES} : Expression{0, CodeRange{}, NO};
}

Expression sqrt(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "sqrt");
    if (!type_check.ok) return type_check.error;
    return makeNumber(::sqrt(getNumber(in)));
}

Expression round(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round");
    if (!type_check.ok) return type_check.error;
    return makeNumber(::round(getNumber(in)));
}

Expression roundUp(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round_up");
    if (!type_check.ok) return type_check.error;
    return makeNumber(ceil(getNumber(in)));
}

Expression roundDown(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round_down");
    if (!type_check.ok) return type_check.error;
    return makeNumber(floor(getNumber(in)));
}

Expression asciiNumber(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, CHARACTER, "ascii_number");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getCharacter(in));
}

Expression asciiCharacter(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "ascii_character");
    if (!type_check.ok) return type_check.error;
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}

}
