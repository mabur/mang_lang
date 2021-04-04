#include "String.h"
#include <algorithm>

#include "Character.h"

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
    auto result = std::make_shared<String>(range(), parent, elements);
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
        value = ::prepend(value, it->character);
    }
    value = ::reverse(value);
    return std::make_shared<String>(CodeRange{first, code.begin()}, nullptr, value);
}

bool String::startsWith(CodeRange code) {
    return ::startsWith(code, '"');
}

bool String::boolean() const {
    return !!elements;
}

ExpressionPointer String::first() const {
    return std::make_shared<Character>(range(), nullptr, elements->first);
}

ExpressionPointer String::rest() const {
    return std::make_shared<String>(range(), nullptr, elements->rest);
}

ExpressionPointer String::reverse() const {
    return std::make_shared<String>(range(), nullptr, ::reverse(elements));
}

ExpressionPointer String::prepend(ExpressionPointer item) const {
    auto new_list = ::prepend(elements, item->character());
    return std::make_shared<String>(range(), nullptr, new_list);
}
