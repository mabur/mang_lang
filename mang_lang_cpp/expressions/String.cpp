#include "String.h"
#include <algorithm>

#include "Character.h"

std::string String::serialize() const {
    auto value = std::string{"\""};
    auto node = list();
    for (; node; node = node->rest) {
        value += node->first->character();
    }
    value += "\"";
    return value;
}

ExpressionPointer String::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<String>(range(), parent, list());
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
    auto value = InternalList{};
    for (auto it = first_character; it != last_character; ++it) {
        auto item = std::make_shared<Character>(
            CodeRange{it, it + 1}, nullptr, it->character
        );
        value = ::prepend<ExpressionPointer>(value, item);
    }
    value = ::reverse(value);
    return std::make_shared<String>(CodeRange{first, code.begin()}, nullptr, value);
}

bool String::startsWith(CodeRange code) {
    return ::startsWith(code, '"');
}
