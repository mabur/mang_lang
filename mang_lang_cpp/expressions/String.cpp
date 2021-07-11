#include "String.h"
#include <algorithm>

#include "Character.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string String::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer String::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
