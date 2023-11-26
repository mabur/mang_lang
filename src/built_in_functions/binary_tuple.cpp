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
    const auto evaluated_tuple = storage.evaluated_tuples.at(in.index);
    const auto tuple_count = evaluated_tuple.last - evaluated_tuple.first;
    if (tuple_count != 2) {
        throw std::runtime_error{
            "I found a dynamic type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got " + std::to_string(tuple_count) + " items."
        };
    }
    const auto left = storage.expressions.at(evaluated_tuple.first + 0);
    const auto right = storage.expressions.at(evaluated_tuple.first + 1);
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
    const auto evaluated_tuple = storage.evaluated_tuples.at(in.index);
    const auto count = evaluated_tuple.last - evaluated_tuple.first;
    if (count != 2) {
        throw std::runtime_error{
            "I found a static type error while calling the function " + function + ". " +
                "The function expected a tuple of two items, " +
                "but it got " + std::to_string(count) + " items."
        };
    }
    const auto left = storage.expressions.at(evaluated_tuple.first + 0);
    const auto right = storage.expressions.at(evaluated_tuple.first + 1);
    return BinaryTuple{left, right};
}
