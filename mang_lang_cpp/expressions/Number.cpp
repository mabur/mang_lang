#include "Number.h"

#include "../operations/boolean.h"
#include "../operations/evaluate.h"
#include "../operations/is_equal.h"
#include "../operations/number.h"
#include "../operations/serialize.h"

std::string Number::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer Number::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluateNumber(*this, environment, log);
}

bool Number::isEqual(const Expression* expression) const {
    return ::isEqual(this, expression);
}

double Number::number() const {
    return ::number(this);
}

bool Number::boolean() const {
    return ::boolean(this);
}
