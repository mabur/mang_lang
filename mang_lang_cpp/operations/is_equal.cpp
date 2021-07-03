#include "is_equal.h"

#include "../expressions/Character.h"
#include "../expressions/Number.h"

bool isEqual(const Number& number, const Expression* expression) {
    try {
        return number.number() == expression->number();
    } catch (...) {
        return false;
    }
}

bool isEqual(const Character& character, const Expression* expression) {
    try {
        return character.character() == expression->character();
    } catch (...) {
        return false;
    }
}
