#include "Character.h"
#include <algorithm>

std::string Character::serialize() const {
    return "\'" + std::string{value} + "\'";
}

ExpressionPointer Character::evaluate(const Expression* environment, std::ostream& log) const {
    auto result = std::make_shared<Character>(range(), environment, value);
    log << result->serialize();
    return result;
}

ExpressionPointer Character::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    const auto value = it->character;
    return std::make_shared<Character>(CodeRange{first, code.begin()}, nullptr, value);
}

bool Character::startsWith(CodeRange code) {
    return ::startsWith(code, '\'');
}

bool Character::isEqual(const Expression* expression) const {
    try {
        return character() == expression->character();
    } catch (...) {
        return false;
    }
}

char Character::character() const {
    return value;
}
