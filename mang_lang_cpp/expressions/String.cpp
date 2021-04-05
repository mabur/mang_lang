#include "String.h"
#include <algorithm>

#include "Character.h"

std::string String::serialize() const {
    auto value = std::string{"\""};
    auto list = elements;
    for (; list; list = list->rest) {
        value += list->first->character();
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

bool String::boolean() const {
    return !!elements;
}

bool String::isEqual(const Expression* expression) const {
    auto left = list();
    auto right = expression->list();
    for (; left && right; left = left->rest, right = right->rest) {
        if (!(left->first)->isEqual(right->first.get())) {
            return false;
        }
    }
    return !left && !right;
}

const InternalList& String::list() const {
    return elements;
}

ExpressionPointer String::first() const {
    return elements->first;
}

ExpressionPointer String::rest() const {
    return std::make_shared<String>(range(), nullptr, elements->rest);
}

ExpressionPointer String::reverse() const {
    return std::make_shared<String>(range(), nullptr, ::reverse(elements));
}

ExpressionPointer String::prepend(ExpressionPointer item) const {
    auto new_list = ::prepend(elements, item);
    return std::make_shared<String>(range(), nullptr, new_list);
}
