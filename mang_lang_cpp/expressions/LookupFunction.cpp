#include "LookupFunction.h"
#include <cassert>

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string LookupFunction::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer LookupFunction::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluateLookupFunction(*this, environment, log);
}
