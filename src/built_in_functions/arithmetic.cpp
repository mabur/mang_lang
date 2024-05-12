#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "binary_tuple.h"
#include "../factory.h"

namespace arithmetic {
namespace {

void checkStaticTypeUnaryFunction(Expression in, ExpressionType expected, const std::string& function) {
    if (in.type != ANY && in.type != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a " + NAMES[expected] + "," +
                "\nbut now got " + NAMES[in.type] +
                ".\n"
        );
    }
}

void checkDynamicTypeUnaryFunction(Expression in, ExpressionType expected, const std::string& function) {
    if (in.type != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a " + NAMES[expected] + "," +
                "\nbut now got " + NAMES[in.type] +
                ".\n"
        );
    }
}

void checkStaticTypeBinaryFunction(Expression in, ExpressionType expected, const std::string& function) {
    const auto tuple = getStaticBinaryTuple(in, function);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[expected] + "s," +
                "\nbut now the first item in the tuple is " + NAMES[left] +
                ".\n"
        );
    }
    if (right != ANY && right != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[expected] + "s," +
                "\nbut now the second item in the tuple is " + NAMES[right] +
                ".\n"
        );
    }
}

void checkDynamicTypeBinaryFunction(Expression in, ExpressionType expected, const std::string& function) {
    const auto tuple = getDynamicBinaryTuple(in, function);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[expected] + "s," +
                "\nbut now the first item in the tuple is " + NAMES[left] +
                ".\n"
        );
    }
    if (right != expected) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens when calling the built-in function " + function + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[expected] + "s," +
                "\nbut now the second item in the tuple is " + NAMES[right] +
                ".\n"
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

Expression mod(Expression in) {
    checkDynamicTypeBinaryFunction(in, NUMBER, "mod");
    return binaryOperation(
        in, [](double a, double b){return std::fmod(a, b);}, "mod"
    );
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

Expression sqrt(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "sqrt");
    return makeNumber(std::sqrt(getNumber(in)));
}

Expression round(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round");
    return makeNumber(std::round(getNumber(in)));
}

Expression round_up(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round_up");
    return makeNumber(std::ceil(getNumber(in)));
}

Expression round_down(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "round_down");
    return makeNumber(std::floor(getNumber(in)));
}

Expression ascii_number(Expression in) {
    checkDynamicTypeUnaryFunction(in, CHARACTER, "ascii_number");
    return makeNumber(getCharacter(in));
}

Expression ascii_character(Expression in) {
    checkDynamicTypeUnaryFunction(in, NUMBER, "ascii_character");
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}

Expression FunctionNumberToNumber::operator()(Expression in) const {
    checkStaticTypeUnaryFunction(in, NUMBER, name);
    return makeNumber(1);
}

Expression FunctionNumberToCharacter::operator()(Expression in) const {
    checkStaticTypeUnaryFunction(in, NUMBER, name);
    return makeCharacter(CodeRange{}, 'a');
}
Expression FunctionCharacterToNumber::operator()(Expression in) const {
    checkStaticTypeUnaryFunction(in, CHARACTER, name);
    return makeNumber(1);
}

Expression FunctionNumberNumberToBoolean::operator()(Expression in) const {
    checkStaticTypeBinaryFunction(in, NUMBER, name);
    return Expression{YES, 0, CodeRange{}};
}

}
