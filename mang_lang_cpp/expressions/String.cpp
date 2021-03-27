#include "String.h"
#include <algorithm>

std::string String::serialize() const {
    return "\"" + value + "\"";
}

ExpressionPointer String::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<String>(begin(), end(), parent, value);
    log << result->serialize();
    return result;
}

bool isNotEndOfString(CodeCharacter c) {
    return c.character != '"';
}

ExpressionPointer String::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '"');
    const auto first_character = code.begin();
    code = parseWhile(code, isNotEndOfString);
    const auto last_character = code.begin();
    code = parseCharacter(code, '"');
    const auto value = rawString({first_character, last_character});
    return std::make_shared<String>(first, code.begin(), nullptr, value);
}

bool String::startsWith(CodeRange code) {
    return ::startsWith(code, '"');
}

bool String::boolean() const {
    return !value.empty();
}
