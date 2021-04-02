#include "String.h"
#include <algorithm>

std::string String::serialize() const {
    auto value = std::string{"\""};
    auto list = elements;
    for (; list; list = list->rest) {
        value += list->first;
    }
    value += "\"";
    return value;
}

ExpressionPointer String::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<String>(begin(), end(), parent, elements);
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
    auto value = InternalString{};
    for (auto it = first_character; it != last_character; ++it) {
        value = prepend(value, it->character);
    }
    value = reverse(value);
    return std::make_shared<String>(first, code.begin(), nullptr, value);
}

bool String::startsWith(CodeRange code) {
    return ::startsWith(code, '"');
}

bool String::boolean() const {
    return !!elements;
}
