#include "Number.h"
#include <algorithm>

std::string Number::serialize() const {
    std::stringstream s;
    s << value;
    return s.str();
}

ExpressionPointer Number::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Number>(begin(), end(), parent, value);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Number::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseOptionalCharacter(it, last, isSign);
    it = parseCharacter(it, last, isDigit);
    it = std::find_if_not(it, last, isDigit);
    it = parseOptionalCharacter(it, last, '.');
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
