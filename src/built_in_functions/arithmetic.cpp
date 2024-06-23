#include "arithmetic.h"

#include <cmath>
#include <string>

#include "binary_tuple.h"
#include "../exceptions.h"
#include "../factory.h"

namespace arithmetic {
namespace {

void checkStaticTypeUnaryFunction(Expression in, ExpressionType expected, const char* function) {
    if (in.type != ANY && in.type != expected) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a %s,\n"
            "but now got %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[in.type].c_str()
        );
    }
}

void checkDynamicTypeUnaryFunction(Expression in, ExpressionType expected, const char* function) {
    if (in.type != expected) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a %s,\n"
            "but now got %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[in.type].c_str()
        );
    }
}

void checkStaticTypeBinaryFunction(Expression in, ExpressionType expected, const char* function) {
    const auto tuple = getStaticBinaryTuple(in, function);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != expected) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the first item in the tuple is %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[left].c_str()
        );
    }
    if (right != ANY && right != expected) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the second item in the tuple is %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[right].c_str()
        );
    }
}

void checkDynamicTypeBinaryFunction(Expression in, ExpressionType expected, const char* function) {
    const auto tuple = getDynamicBinaryTuple(in, function);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != expected) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the first item in the tuple is %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[left].c_str()
        );
    }
    if (right != ANY && right != expected) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens when calling the built-in function %s.\n"
            "The function expects to be called with a tuple of two %ss,\n"
            "but now the second item in the tuple is %s.\n",
            function,
            NAMES[expected].c_str(),
            NAMES[right].c_str()
        );
    }
}

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

template <typename BinaryOperation>
Expression binaryOperation(Expression in, BinaryOperation operation, const std::string& function) {
    const auto tuple = getDynamicBinaryTuple(in, function);
    const auto left = getNumber(tuple.left);
    const auto right = getNumber(tuple.right);
    return makeNumber(operation(left, right));
}

} // namespace

Expression add(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "add");
    return binaryOperation(in, std::plus<>(), "add");
}

Expression addTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "add");
    return makeNumber(1);
}

Expression mul(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "mul");
    return binaryOperation(in, std::multiplies<>(), "mul");
}

Expression mulTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "mul");
    return makeNumber(1);
}

Expression sub(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "sub");
    return binaryOperation(in, std::minus<>(), "sub");
}

Expression subTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "sub");
    return makeNumber(1);
}

Expression div(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "div");
    return binaryOperation(in, std::divides<>(), "div");
}

Expression divTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "div");
    return makeNumber(1);
}

double myMod(double a, double b) {
    return std::fmod(a, b);
}

Expression mod(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "mod");
    return binaryOperation(in, myMod, "mod");
}

Expression modTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "mod");
    return makeNumber(1);
}

Expression less(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "less");
    const auto tuple = getDynamicBinaryTuple(in, "less");
    const auto left = getNumber(tuple.left);
    const auto right = getNumber(tuple.right);
    return left < right ?
        Expression{YES, 0, CodeRange{}} : Expression{NO, 0, CodeRange{}};
}

Expression lessTyped(Expression in) {
    checkStaticTypeBinaryFunction(in, NUMBER, "less");
    return Expression{YES, 0, CodeRange{}};
}

Expression sqrt(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "sqrt");
    return makeNumber(std::sqrt(getNumber(in)));
}

Expression sqrtTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, NUMBER, "sqrt");
    return makeNumber(1);
}

Expression round(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round");
    return makeNumber(std::round(getNumber(in)));
}

Expression roundTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, NUMBER, "round");
    return makeNumber(1);
}

Expression roundUp(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round_up");
    return makeNumber(std::ceil(getNumber(in)));
}

Expression roundUpTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, NUMBER, "round_up");
    return makeNumber(1);
}

Expression roundDown(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round_down");
    return makeNumber(std::floor(getNumber(in)));
}

Expression roundDownTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, NUMBER, "round_down");
    return makeNumber(1);
}

Expression asciiNumber(Expression in) {
    checkDynamicTypeUnaryFunction(in, CHARACTER, "ascii_number");
    return makeNumber(getCharacter(in));
}

Expression asciiNumberTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, CHARACTER, "ascii_number");
    return makeNumber(1);
}

Expression asciiCharacter(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "ascii_character");
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}

Expression asciiCharacterTyped(Expression in) {
    checkStaticTypeUnaryFunction(in, NUMBER, "ascii_character");
    return makeCharacter(CodeRange{}, 'a');
}

}
