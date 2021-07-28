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
    const auto left = left_smart.get();
    const auto right = right_smart.get();
    if (left->type_ == NUMBER) {
        try {
            return ::number(left) == ::number(right);
        } catch (...) {
            return false;
        }
    }
    if (left->type_ == CHARACTER) {
        try {
            return ::character(left) == ::character(right);
        } catch (...) {
            return false;
        }
    }
    if (left->type_ == LIST) {
        return isEqualList(left_smart, right_smart);
    }
    if (left->type_ == STRING) {
        return isEqualList(left_smart, right_smart);
    }
    return false;
}
