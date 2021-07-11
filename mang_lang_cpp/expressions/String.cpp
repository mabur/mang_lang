#include "String.h"
#include <algorithm>

#include "Character.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer String::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
