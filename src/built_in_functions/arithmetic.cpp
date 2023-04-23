#include "arithmetic.h"

#include <cmath>
#include <functional>

#include "../factory.h"

namespace arithmetic {
namespace {

Expression makeNumber(double x) {
    return makeNumber(CodeRange{}, x);
}

Expression makeBoolean(bool x) {
    return makeBoolean(CodeRange{}, x);
}

template <typename BinaryOperation>
Expression binaryOperation(Expression in, BinaryOperation operation) {
    const auto tuple = getBinaryTuple(in);
    return makeNumber(operation(getNumber(tuple.left), getNumber(tuple.right)));
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

Expression mod(Expression in) {
    return binaryOperation(
        in, [](double a, double b){return std::fmod(a, b);}
    );
}

Expression less(Expression in) {
    const auto tuple = getBinaryTuple(in);
    return makeBoolean(getNumber(tuple.left) < getNumber(tuple.right));
}

Expression sqrt(Expression in) {
    return makeNumber(std::sqrt(getNumber(in)));
}

Expression round(Expression in) {
    return makeNumber(std::round(getNumber(in)));
}

Expression round_up(Expression in) {
    return makeNumber(std::ceil(getNumber(in)));
}

Expression round_down(Expression in) {
    return makeNumber(std::floor(getNumber(in)));
}

Expression ascii_number(Expression in) {
    return makeNumber(getCharacter(in));
}

Expression ascii_character(Expression in) {
    return makeCharacter(CodeRange{}, static_cast<char>(getNumber(in)));
}

Expression FunctionNumberToNumber::operator()(Expression in) const {
    if (in.type != ANY && in.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a " + NAMES[NUMBER] + "," +
                "\nbut now got " + NAMES[in.type] +
                ".\n"
        );
    }
    return makeNumber(1);
}
Expression FunctionNumberToCharacter::operator()(Expression in) const {
    if (in.type != ANY && in.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a " + NAMES[NUMBER] + "," +
                "\nbut now got " + NAMES[in.type] +
                ".\n"
        );
    }
    return makeCharacter(CodeRange{}, 'a');
}
Expression FunctionCharacterToNumber::operator()(Expression in) const {
    if (in.type != ANY && in.type != CHARACTER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a " + NAMES[CHARACTER] + "," +
                "\nbut now got " + NAMES[in.type] +
                ".\n"
        );
    }
    return makeNumber(1);
}

Expression FunctionNumberNumberToBoolean::operator()(Expression in) const {
    const auto tuple = getBinaryTuple(in);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[NUMBER] + "s," +
                "\nbut now the first item in the tuple is " + NAMES[left] +
                ".\n"
        );
    }
    if (right != ANY && right != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[NUMBER] + "s," +
                "\nbut now the second item in the tuple is " + NAMES[right] +
                ".\n"
        );
    }
    return makeBoolean(true);
}

Expression FunctionNumberNumberToNumber::operator()(Expression in) const {
    const auto tuple = getBinaryTuple(in);
    const auto left = tuple.left.type;
    const auto right = tuple.right.type;
    if (left != ANY && left != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[NUMBER] + "s," +
                "\nbut now the first item in the tuple is " + NAMES[left] +
                ".\n"
        );
    }
    if (right != ANY && right != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens when calling the built-in function " + name + ". " +
                "\nThe function expects to be called with a tuple of two " + NAMES[NUMBER] + "s," +
                "\nbut now the second item in the tuple is " + NAMES[right] +
                ".\n"
        );
    }
    return makeNumber(1);
}

}
