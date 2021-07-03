#include "String.h"
#include <algorithm>

#include "Character.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string String::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer String::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
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
