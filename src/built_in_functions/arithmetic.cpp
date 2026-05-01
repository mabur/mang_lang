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

TypeCheck checkTypeBinaryFunction(Expression in, ExpressionType expected, const char* function) {
    auto result = MAKE(TypeCheck, .ok=true);
    const auto tuple = getStaticBinaryTuple(in, function);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != expected) {
        result.ok = false;
        result.error = makeEvaluateError({}, format_cstring(
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the first item in the tuple is %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(left)
        ));
        return result;
    }
    if (right != ANY && right != expected) {
        result.ok = false;
        result.error = makeEvaluateError({}, format_cstring(
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the second item in the tuple is %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(right)
        ));
        return result;
    }
    return result;
}

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

} // namespace

#define MAKE_BINARY_ARITHMETIC_OPERATION(tuple, op) (makeNumber(getNumber((tuple).left) op getNumber((tuple).right)))

Expression add(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "add");
    if (!type_check.ok) return type_check.error;
    auto tuple = getDynamicBinaryTuple(in, "add");
    return MAKE_BINARY_ARITHMETIC_OPERATION(tuple, +);
}

Expression mul(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "mul");
    if (!type_check.ok) return type_check.error;
    auto tuple = getDynamicBinaryTuple(in, "mul");
    return MAKE_BINARY_ARITHMETIC_OPERATION(tuple, *);
}

Expression sub(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "sub");
    if (!type_check.ok) return type_check.error;
    auto tuple = getDynamicBinaryTuple(in, "sub");
    return MAKE_BINARY_ARITHMETIC_OPERATION(tuple, -);
}

Expression div(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "div");
    if (!type_check.ok) return type_check.error;
    auto tuple = getDynamicBinaryTuple(in, "div");
    return MAKE_BINARY_ARITHMETIC_OPERATION(tuple, /);
}

Expression mod(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "mod");
    if (!type_check.ok) return type_check.error;
    auto tuple = getDynamicBinaryTuple(in, "mod");
    return makeNumber(fmod(getNumber(tuple.left), getNumber(tuple.right)));
}

Expression less(Expression in) {
    auto type_check = checkTypeBinaryFunction(in, NUMBER, "less");
    if (!type_check.ok) return type_check.error;
    const auto tuple = getDynamicBinaryTuple(in, "less");
    const auto left = getNumber(tuple.left);
    const auto right = getNumber(tuple.right);
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
