#include "Number.h"
#include <algorithm>

std::string Number::serialize() const {
    std::stringstream s;
    s << value;
    return s.str();
}

ExpressionPointer Number::evaluate(const Expression* parent) const {
    return std::make_shared<Number>(begin(), end(), parent, value);
}

ExpressionPointer Number::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseOptionalCharacter(it, isSign);
    it = parseCharacter(it, isDigit);
    it = std::find_if_not(it, last, isDigit);
    it = parseOptionalCharacter(it, '.');
    it = std::find_if_not(it, last, isDigit);
    const auto value = std::stod(rawString(first, it));
    return std::make_shared<Number>(first, it, nullptr, value);
}

bool Number::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return isSign(*first) || isDigit(*first);
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
