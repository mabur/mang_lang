#include "binary_tuple.h"

#include "../exceptions.h"
#include "../factory.h"

BinaryTuple getDynamicBinaryTuple(Expression in, const char* function) {
    if (in.type != EVALUATED_TUPLE) {
        throwException(
            "I found a dynamic type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got a %s",
            function,
            NAMES[in.type].c_str()
        );
    }
    const auto evaluated_tuple = storage.evaluated_tuples.data[in.index];
    const auto count = evaluated_tuple.last - evaluated_tuple.first;
    if (count != 2) {
        throwException(
            "I found a dynamic type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got %zu items.",
            function,
            count
        );
    }
    const auto left = storage.expressions.data[evaluated_tuple.first + 0];
    const auto right = storage.expressions.data[evaluated_tuple.first + 1];
    return BinaryTuple{left, right};
}

BinaryTuple getStaticBinaryTuple(Expression in, const char* function) {
    if (in.type != EVALUATED_TUPLE) {
        throwException(
            "I found a static type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got a %s",
            function,
            NAMES[in.type].c_str()
        );
    }
    const auto evaluated_tuple = storage.evaluated_tuples.data[in.index];
    const auto count = evaluated_tuple.last - evaluated_tuple.first;
    if (count != 2) {
        throwException(
            "I found a static type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got %zu items.",
            function,
            count
        );
    }
    const auto left = storage.expressions.data[evaluated_tuple.first + 0];
    const auto right = storage.expressions.data[evaluated_tuple.first + 1];
    return BinaryTuple{left, right};
}
