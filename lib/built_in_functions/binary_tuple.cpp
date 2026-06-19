#include "binary_tuple.h"

#include "../factory.h"
#include "../mang_lang_string.h"

BinaryTuple getBinaryTuple(Expression in, const char* function) {
    auto result = MAKE(BinaryTuple);
    if (in.type != EVALUATED_TUPLE) {
        result.error = makeErrorExpression(
            in.range,
            "I found a type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got a %s",
            function,
            getExpressionName(in.type)
        );
        return result;
    }
    const auto evaluated_tuple = storage.evaluated_tuples.data[in.index];
    const auto count = evaluated_tuple.indices.count;
    if (count != 2) {
        result.error = makeErrorExpression(
            in.range,
            "I found a type error while calling the function %s. "
            "The function expected a tuple of two items, "
            "but it got %zu items.",
            function,
            count
        );
        return result;
    }
    result.left = storage.expressions.data[evaluated_tuple.indices.data + 0];
    result.right = storage.expressions.data[evaluated_tuple.indices.data + 1];
    result.ok = true;
    return result;
}
