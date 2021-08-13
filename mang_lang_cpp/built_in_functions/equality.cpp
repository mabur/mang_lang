#include "equality.h"

#include "../Expression.h"
#include "../factory.h"
#include "../operations/character.h"
#include "../operations/list.h"
#include "../operations/number.h"

namespace equality {

bool isEqual(ExpressionPointer left_smart, ExpressionPointer right_smart);

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

bool isEqual(ExpressionPointer left_smart, ExpressionPointer right_smart) {
    const auto left_type = left_smart.type;
    const auto right_type = right_smart.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return ::number(left_smart) == ::number(right_smart);
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return ::character(left_smart) == ::character(right_smart);
    }
    if (left_type == LIST && right_type == LIST) {
        return isEqualList(left_smart, right_smart);
    }
    if (left_type == STRING && right_type == STRING) {
        return isEqualList(left_smart, right_smart);
    }
    return false;
}

ExpressionPointer makeNumber(double x) {
    return makeNumber(new Number(Number{CodeRange{}, x}));
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
