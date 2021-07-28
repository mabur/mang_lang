#include "is_equal.h"

#include "character.h"
#include "list.h"
#include "number.h"

bool isEqualList(const ExpressionPointer& left_smart, const ExpressionPointer& right_smart) {
    auto left = list(left_smart.get());
    auto right = list(right_smart.get());
    for (; left && right; left = left->rest, right = right->rest) {
        if (!isEqual(left->first, right->first)) {
            return false;
        }
    }
    return !left && !right;
}

bool isEqual(const ExpressionPointer& left_smart, const ExpressionPointer& right_smart) {
    const auto left_type = left_smart.get()->type_;
    const auto right_type = right_smart.get()->type_;
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
