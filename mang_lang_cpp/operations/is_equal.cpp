#include "is_equal.h"

#include "../expressions/Character.h"
#include "../expressions/Number.h"

bool isEqualNumber(const Number& number, const Expression* expression) {
    try {
        return number.number() == expression->number();
    } catch (...) {
        return false;
    }
}

bool isEqualCharacter(const Character& character, const Expression* expression) {
    try {
        return character.character() == expression->character();
    } catch (...) {
        return false;
    }
}

bool isEqualList(const Expression& list, const Expression* expression) {
    auto left = list.list();
    auto right = expression->list();
    for (; left && right; left = left->rest, right = right->rest) {
        if (!(left->first)->isEqual(right->first.get())) {
            return false;
        }
    }
    return !left && !right;
}

bool isEqual(const Expression* left, const Expression* right) {
    if (left->type_ == NUMBER) {
        return isEqualNumber(*dynamic_cast<const Number *>(left), right);
    }
    if (left->type_ == CHARACTER) {
        return isEqualCharacter(*dynamic_cast<const Character *>(left), right);
    }
    if (left->type_ == LIST) {
        return isEqualList(*left, right);
    }
    if (left->type_ == STRING) {
        return isEqualList(*left, right);
    }
    throw std::runtime_error{"Cannot compare expression"};
}
