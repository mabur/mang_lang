#include "String.h"
#include <algorithm>

std::string String::serialize() const {
    return "\"" + value + "\"";
}

ExpressionPointer String::evaluate(const Expression* parent) const {
    return std::make_shared<String>(begin(), end(), parent, value);
}

ExpressionPointer String::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '"');
    const auto first_character = it;
    it = std::find_if(it, last, isStringSeparator);
    const auto last_character = it;
    it = parseCharacter(it, '"');
    const auto value = rawString(first_character, last_character);
    return std::make_shared<String>(first, it, nullptr, value);
}
