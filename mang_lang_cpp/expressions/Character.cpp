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
