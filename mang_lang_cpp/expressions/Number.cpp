#include "Number.h"

#include "../operations/evaluate.h"

std::string Number::serialize() const {
    std::stringstream s;
    s << value;
    return s.str();
}

ExpressionPointer Number::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

ExpressionPointer Number::parse(CodeRange code) {
    auto first = code.begin();
    code = parseOptionalCharacter(code, isSign);
    code = parseCharacter(code, isDigit);
    code = parseWhile(code, isDigit);
    code = parseOptionalCharacter(code, '.');
    code = parseWhile(code, isDigit);
    const auto value = std::stod(rawString({first, code.first}));
    return std::make_shared<Number>(CodeRange{first, code.first}, nullptr, value);
}

bool Number::startsWith(CodeRange code) {
    return !code.empty() && (isSign(*code.begin()) || isDigit(*code.begin()));
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
