#include "Name.h"

std::string Name::serialize() const {
    return value;
}

ExpressionPointer Name::evaluate(const Expression* parent) const {
    return std::make_shared<Name>(begin(), end(), parent, value);
}
