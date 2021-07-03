#include "Number.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string Number::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer Number::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

bool Number::isEqual(const Expression* expression) const {
    try {
        return number() == expression->number();
    } catch (...) {
        return false;
    }
}

double Number::number() const {
    return value;
}

bool Number::boolean() const {
    return static_cast<bool>(value);
}
