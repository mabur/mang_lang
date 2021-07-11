#include "Character.h"
#include <algorithm>

#include "../operations/character.h"
#include "../operations/evaluate.h"
#include "../operations/is_equal.h"
#include "../operations/parse.h"
#include "../operations/serialize.h"

ExpressionPointer Character::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}

bool Character::isEqual(const Expression* expression) const {
    return ::isEqual(this, expression);
}

char Character::character() const {
    return ::character(this);
}
