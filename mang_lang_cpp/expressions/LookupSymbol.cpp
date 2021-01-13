#include "LookupSymbol.h"
#include <algorithm>

std::string LookupSymbol::serialize() const {
    return name.serialize();
}

ExpressionPointer LookupSymbol::evaluate(const Expression* parent) const {
    return ExpressionPointer{parent->lookup(name.value)};
}

ExpressionPointer LookupSymbol::parse(const CodeCharacter* first, const CodeCharacter* last) {
    const auto name = Name::parse(first, last);
    return std::make_shared<LookupSymbol>(first, name.end(), nullptr, name);
}

bool LookupSymbol::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return std::isalpha(first->character);
}
