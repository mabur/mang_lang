#include "binary_tuple.h"

#include "../factory.h"

BinaryTuple getDynamicBinaryTuple(Expression in, const std::string& function) {
    if (in.type != EVALUATED_TUPLE) {
        throw std::runtime_error{
            "I found a dynamic type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got a " + NAMES[in.type]
        };
    }
    const auto& expressions = storage.evaluated_tuples.at(in.index).expressions;
    if (expressions.size() != 2) {
        throw std::runtime_error{
            "I found a dynamic type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got " + std::to_string(expressions.size()) + " items."
        };
    }
    const auto left = expressions.at(0);
    const auto right = expressions.at(1);
    return BinaryTuple{left, right};
}

BinaryTuple getStaticBinaryTuple(Expression in, const std::string& function) {
    if (in.type != EVALUATED_TUPLE) {
        throw std::runtime_error{
            "I found a static type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got a " + NAMES[in.type]
        };
    }
    const auto& expressions = storage.evaluated_tuples.at(in.index).expressions;
    if (expressions.size() != 2) {
        throw std::runtime_error{
            "I found a static type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got " + std::to_string(expressions.size()) + " items."
        };
    }
    const auto left = expressions.at(0);
    const auto right = expressions.at(1);
    return BinaryTuple{left, right};
}
