#include "String.h"

std::string String::serialize() const {
    return "\"" + value + "\"";
}

ExpressionPointer String::evaluate(const Expression* parent) const {
    return std::make_shared<String>(begin(), end(), parent, value);
}
