#include "logic.h"

#include "../factory.h"
#include "../operations/boolean.h"

namespace logic {

InternalList list(ExpressionPointer expression) {
    switch (expression.type) {
        case LIST: return getList(expression).elements;
        case STRING: return getString(expression).elements;
        default: throw std::runtime_error{"Expected list"};
    }
}

bool isTrue(ExpressionPointer x) {
    return ::boolean(x);
}

bool isFalse(ExpressionPointer x) {
    return !::boolean(x);
}

ExpressionPointer makeNumber(double x) {
    return makeNumber(new Number(Number{CodeRange{}, x}));
}

ExpressionPointer boolean(ExpressionPointer in) {
    return makeNumber(::boolean(in));
}

ExpressionPointer logic_not(ExpressionPointer in) {
    return makeNumber(!::boolean(in));
}

ExpressionPointer all(ExpressionPointer in) {
    const auto result = !findIf(list(in), isFalse);
    return makeNumber(result);
}

ExpressionPointer any(ExpressionPointer in) {
    const auto result = !!findIf(list(in), isTrue);
    return makeNumber(result);
}

ExpressionPointer none(ExpressionPointer in) {
    const auto result = !findIf(list(in), isTrue);
    return makeNumber(result);
}

bool isEqual(ExpressionPointer left, ExpressionPointer right);

bool isEqualList(ExpressionPointer left_smart, ExpressionPointer right_smart) {
    auto left = list(left_smart);
    auto right = list(right_smart);
    for (; left && right; left = left->rest, right = right->rest) {
        if (!isEqual(left->first, right->first)) {
            return false;
        }
    }
    return !left && !right;
}

bool isEqual(ExpressionPointer left, ExpressionPointer right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left).value == getNumber(right).value;
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left).value == getCharacter(right).value;
    }
    if (left_type == LIST && right_type == LIST) {
        return isEqualList(left, right);
    }
    if (left_type == STRING && right_type == STRING) {
        return isEqualList(left, right);
    }
    return false;
}

ExpressionPointer equal(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = isEqual(left, right);
    return makeNumber(value);
}

ExpressionPointer unequal(ExpressionPointer in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = !isEqual(left, right);
    return makeNumber(value);
}

}
