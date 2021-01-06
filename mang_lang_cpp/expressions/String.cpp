#include "String.h"
#include <algorithm>

std::string String::serialize() const {
    return "\"" + value + "\"";
}

ExpressionPointer String::evaluate(const Expression* parent) const {
    return std::make_shared<String>(begin(), end(), parent, value);
}

bool endsWith(CodeCharacter c) {
    return c.character == '"';
}

ExpressionPointer String::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '"');
    const auto first_character = it;
    it = std::find_if(it, last, endsWith);
    const auto last_character = it;
    it = parseCharacter(it, '"');
    const auto value = rawString(first_character, last_character);
    return std::make_shared<String>(first, it, nullptr, value);
}

bool String::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    return first->character == '"';
}

bool String::boolean() const {
    return !value.empty();
}
