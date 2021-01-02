#include "LookupSymbol.h"
#include <algorithm>
#include "LookupChild.h"

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
    it = parseWhiteSpace(it, last);
    if (!isChildLookup(*it)) {
        return std::make_shared<LookupSymbol>(first, it, nullptr, name);
    } else {
        it = parseCharacter(it, '<');
        it = parseWhiteSpace(it, last);
        auto child = Expression::parse(it, last);
        it = child->end();
        return std::make_shared<LookupChild>(first, it, nullptr, name, std::move(child));
    }
}

bool LookupSymbol::startsWith(CodeCharacter c) {
    return std::isalpha(c.character);
}
