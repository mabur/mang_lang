#include "arithmetic.h"

#include <math.h>

#include "binary_tuple.h"
#include "../factory.h"
#include "../mang_lang_string.h"
#include "../type_check.h"

static
TypeCheck checkTypeUnaryFunction(Expression in, ExpressionType expected, const char* function) {
    auto result = MAKE(TypeCheck, .ok=true);
    if (in.type != ANY_VALUE && in.type != expected) {
        result.ok = false;
        result.error = makeErrorValue({},
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a %s,\n"
            "but now got %s.\n",
            function,
            getExpressionName(expected),
            getExpressionName(in.type)
        );
    }
    return result;
}
    
static
bool isNumberOrAny(Expression expression) {
    return expression.type == NUMBER || expression.type == ANY_VALUE;
} 

static
TypeCheck checkTypeBinaryFunction(Expression left, Expression right, const char* function) {
    auto result = MAKE(TypeCheck, .ok=true);
    if (!isNumberOrAny(left) || !isNumberOrAny(right)) {
        result.ok = false;
        result.error = makeErrorValue({},
            "\n\nI have found a type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two NUMBERs,\n"
            "but now it got (%s %s).\n",
            function,
            getExpressionName(left.type),
            getExpressionName(right.type)
        );
    }
    return result;
}

static
Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

Expression builtInAdd2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "add");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(left) + getNumber(right));
}

Expression builtInMul2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "mul");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(left) * getNumber(right));
}

Expression builtInSub2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "sub");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(left) - getNumber(right));
}

Expression builtInDiv2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "div");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getNumber(left) / getNumber(right));
}

Expression builtInMod2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "mod");
    if (!type_check.ok) return type_check.error;
    return makeNumber(fmod(getNumber(left), getNumber(right)));
}

Expression builtInLess2(Expression left, Expression right) {
    auto type_check = checkTypeBinaryFunction(left, right, "less");
    if (!type_check.ok) return type_check.error;
    return getNumber(left) < getNumber(right) ?
        Expression{0, CodeRange{}, YES} : Expression{0, CodeRange{}, NO};
}

Expression builtInAdd(Expression in) {
    return applyBinaryTuple(in, "add", builtInAdd2);
}

Expression builtInMul(Expression in) {
    return applyBinaryTuple(in, "mul", builtInMul2);
}

Expression builtInSub(Expression in) {
    return applyBinaryTuple(in, "sub", builtInSub2);
}

Expression builtInDiv(Expression in) {
    return applyBinaryTuple(in, "div", builtInDiv2);
}

Expression builtInMod(Expression in) {
    return applyBinaryTuple(in, "mod", builtInMod2);
}

Expression builtInLess(Expression in) {
    return applyBinaryTuple(in, "less", builtInLess2);
}

Expression builtInSqrt(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "sqrt");
    if (!type_check.ok) return type_check.error;
    return makeNumber(::sqrt(getNumber(in)));
}

Expression builtInRound(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round");
    if (!type_check.ok) return type_check.error;
    return makeNumber(::round(getNumber(in)));
}

Expression builtInRoundUp(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round_up");
    if (!type_check.ok) return type_check.error;
    return makeNumber(ceil(getNumber(in)));
}

Expression builtInRoundDown(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "round_down");
    if (!type_check.ok) return type_check.error;
    return makeNumber(floor(getNumber(in)));
}

Expression builtInAsciiNumber(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, CHARACTER, "ascii_number");
    if (!type_check.ok) return type_check.error;
    return makeNumber(getCharacter(in));
}

Expression builtInAsciiCharacter(Expression in) {
    auto type_check = checkTypeUnaryFunction(in, NUMBER, "ascii_character");
    if (!type_check.ok) return type_check.error;
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}
