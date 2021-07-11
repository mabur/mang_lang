#include "LookupSymbol.h"
#include <algorithm>

#include "Name.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string LookupSymbol::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer LookupSymbol::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluateLookupSymbol(*this, environment, log);
}
