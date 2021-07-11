#include "List.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer List::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
