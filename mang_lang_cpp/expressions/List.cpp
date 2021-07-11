#include "List.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string List::serialize() const {
    return ::serialize(this);
}

ExpressionPointer List::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
