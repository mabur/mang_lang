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

ExpressionPointer Number::parse(CodeRange code) {
    auto first = code.begin();
    code = parseOptionalCharacter(code, isSign);
    code = parseCharacter(code, isDigit);
    code.first = std::find_if_not(code.begin(), code.end(), isDigit);
    code = parseOptionalCharacter(code, '.');
    code.first = std::find_if_not(code.begin(), code.end(), isDigit);
    const auto value = std::stod(rawString({first, code.first}));
    return std::make_shared<Number>(first, code.first, nullptr, value);
}

bool Number::startsWith(CodeRange code) {
    return isSign(*code.begin()) || isDigit(*code.begin());
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
