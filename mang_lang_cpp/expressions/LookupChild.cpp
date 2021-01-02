#include "LookupChild.h"

std::string LookupChild::serialize() const {
    return name + "<" + child->serialize();
}

ExpressionPointer LookupChild::evaluate(const Expression* parent) const {
    const auto evaluated_child = child->evaluate(parent);
    return ExpressionPointer{evaluated_child->lookup(name)};
}
