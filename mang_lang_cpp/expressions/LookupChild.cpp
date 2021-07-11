#include "LookupChild.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer LookupChild::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
