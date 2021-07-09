#include "is_equal.h"

#include "../expressions/Character.h"
#include "../expressions/Number.h"

bool isEqualNumber(const Number* number, const Expression* expression) {
    try {
        return number->number() == expression->number();
    } catch (...) {
        return false;
    }
}

bool isEqualCharacter(const Character* character, const Expression* expression) {
    try {
        return character->character() == expression->character();
    } catch (...) {
        return false;
    }
}

bool isEqualList(const Expression* left_expression, const Expression* right_expression) {
    auto left = left_expression->list();
    auto right = right_expression->list();
    for (; left && right; left = left->rest, right = right->rest) {
        if (!(left->first)->isEqual(right->first.get())) {
            return false;
        }
    }
    return !left && !right;
}

bool isEqual(const Expression* left, const Expression* right) {
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
