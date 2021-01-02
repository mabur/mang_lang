#include "LookupSymbol.h"
#include <algorithm>

std::string LookupSymbol::serialize() const {
    return value;
}

ExpressionPointer LookupSymbol::evaluate(const Expression* parent) const {
    return ExpressionPointer{parent->lookup(value)};
}

ExpressionPointer LookupSymbol::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, isLetter);
    it = std::find_if_not(it, last, isNameCharacter);
    const auto name = rawString(first, it);

    return std::make_shared<LookupSymbol>(first, it, nullptr, name);
}

bool LookupSymbol::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    return std::isalpha(first->character);
}
