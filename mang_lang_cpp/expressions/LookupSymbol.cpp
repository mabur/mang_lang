#include "LookupSymbol.h"

std::string LookupSymbol::serialize() const {
    return value;
}

ExpressionPointer LookupSymbol::evaluate(const Expression* parent) const {
    return ExpressionPointer{parent->lookup(value)};
}
