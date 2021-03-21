#include "LookupSymbol.h"
#include <algorithm>

std::string LookupSymbol::serialize() const {
    return name.serialize();
}

ExpressionPointer LookupSymbol::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = ExpressionPointer{parent->lookup(name.value)};
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer LookupSymbol::parse(const CodeCharacter* first, const CodeCharacter* last) {
    const auto name = Name::parse(first, last);
    return std::make_shared<LookupSymbol>(first, name.end(), nullptr, name);
}

bool LookupSymbol::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    if (isAnyKeyword(first, last, KEYWORDS)) {
        return false;
    }
    return std::isalpha(first->character);
}
