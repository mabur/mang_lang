#include "LookupFunction.h"
#include <cassert>

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer LookupFunction::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
