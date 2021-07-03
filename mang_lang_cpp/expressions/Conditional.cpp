#include "Conditional.h"
#include "../parsing.h"

#include "../operations/evaluate.h"
#include "../operations/parse.h"
#include "../operations/serialize.h."

std::string Conditional::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer Conditional::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}
