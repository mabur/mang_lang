#include "is_equal.h"

#include "../expressions/Character.h"
#include "../expressions/Number.h"

#include "character.h"
#include "list.h"
#include "number.h"

bool isEqualNumber(const Number* number, const Expression* expression) {
    try {
        return ::number(number) == ::number(expression);
    } catch (...) {
        return false;
    }
}

bool isEqualCharacter(const Character* character, const Expression* expression) {
    try {
        return ::character(character) == ::character(expression);
    } catch (...) {
        return false;
    }
}

bool isEqualList(const Expression* left_expression, const Expression* right_expression) {
    auto left = list(left_expression);
    auto right = list(right_expression);
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
        return isEqualNumber(dynamic_cast<const Number *>(left), right);
    }
    if (left->type_ == CHARACTER) {
        return isEqualCharacter(dynamic_cast<const Character *>(left), right);
    }
    if (left->type_ == LIST) {
        return isEqualList(left, right);
    }
    if (left->type_ == STRING) {
        return isEqualList(left, right);
    }
    return false;
}
