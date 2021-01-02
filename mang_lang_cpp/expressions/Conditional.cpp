#include "Conditional.h"

std::string Conditional::serialize() const {
    return std::string{"if "} + expression_if->serialize()
        + " then " + expression_then->serialize()
        + " else " + expression_else->serialize();
}

ExpressionPointer Conditional::evaluate(const Expression* parent) const {
    if (expression_if->evaluate(parent)->isTrue()) {
        return expression_then->evaluate(parent);
    } else {
        return expression_else->evaluate(parent);
    }
}
