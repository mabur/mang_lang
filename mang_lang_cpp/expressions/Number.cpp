#include "Number.h"

std::string Number::serialize() const {
    std::stringstream s;
    s << value;
    return s.str();
}

ExpressionPointer Number::evaluate(const Expression* parent) const {
    return std::make_shared<Number>(begin(), end(), parent, value);
}

bool Number::isTrue() const {
    return static_cast<bool>(value);
}
