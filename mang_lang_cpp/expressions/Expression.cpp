#include "Expression.h"

const CodeCharacter* Expression::begin() const {
    return first_;
}

const CodeCharacter* Expression::end() const {
    return last_;
}

const Expression* Expression::parent() const {
    return parent_;
}

ExpressionPointer Expression::lookup(const std::string& name) const {
    if (parent()) {
        return parent()->lookup(name);
    }
    return {};
}

bool Expression::isTrue() const {
    return false;
}
