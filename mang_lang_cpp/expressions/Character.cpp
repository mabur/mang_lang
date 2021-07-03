#include "Character.h"
#include <algorithm>

#include "../operations/evaluate.h"
#include "../operations/parse.h"
#include "../operations/serialize.h"

std::string Character::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer Character::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

bool Character::startsWith(CodeRange code) {
    return ::startsWith(code, '\'');
}

bool Character::isEqual(const Expression* expression) const {
    try {
        return character() == expression->character();
    } catch (...) {
        return false;
    }
}

char Character::character() const {
    return value;
}
