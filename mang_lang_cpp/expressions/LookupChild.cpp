#include "LookupChild.h"
#include <algorithm>

std::string LookupChild::serialize() const {
    return name.serialize() + "<" + child->serialize();
}

ExpressionPointer LookupChild::evaluate(const Expression* parent) const {
    const auto evaluated_child = child->evaluate(parent);
    return ExpressionPointer{evaluated_child->lookup(name.value)};
}

ExpressionPointer LookupChild::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto name = Name::parse(first, last);
    auto it = name.end();
    it = parseWhiteSpace(it, last);
    it = parseCharacter(it, '<');
    it = parseWhiteSpace(it, last);
    auto child = Expression::parse(it, last);
    it = child->end();
    return std::make_shared<LookupChild>(first, it, nullptr, name, std::move(child));
}

bool LookupChild::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    if (!std::isalpha(first->character)) {
        return false;
    }
    auto it = first;
    it = std::find_if_not(it, last, isNameCharacter);
    it = std::find_if_not(it, last, isWhiteSpace);
    return it->character == '<';
}
