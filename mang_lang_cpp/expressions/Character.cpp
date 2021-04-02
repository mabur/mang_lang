#include "Character.h"
#include <algorithm>

std::string Character::serialize() const {
    return "\'" + std::string{value} + "\'";
}

ExpressionPointer Character::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Character>(begin(), end(), parent, value);
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
    return std::make_shared<Character>(first, code.begin(), nullptr, value);
}

bool Character::startsWith(CodeRange code) {
    return ::startsWith(code, '\'');
}

char Character::character() const {
    return value;
}
