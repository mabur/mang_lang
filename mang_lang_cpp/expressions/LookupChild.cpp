#include "LookupChild.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string LookupChild::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer LookupChild::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluateLookupChild(*this, environment, log);
}
