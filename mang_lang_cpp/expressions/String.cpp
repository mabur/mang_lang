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

bool endsWith(CodeCharacter c) {
    return c.character == '"';
}

ExpressionPointer String::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '"');
    const auto first_character = code.begin();
    code.first = std::find_if(code.begin(), code.end(), endsWith);
    const auto last_character = code.begin();
    code = parseCharacter(code, '"');
    const auto value = rawString({first_character, last_character});
    return std::make_shared<String>(first, code.begin(), nullptr, value);
}

bool String::startsWith(CodeRange code) {
    return startsWidth(code, '"');
}

bool String::boolean() const {
    return !value.empty();
}
