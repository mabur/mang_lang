#include "LookupChild.h"
#include <algorithm>

std::string LookupChild::serialize() const {
    return name + "<" + child->serialize();
}

ExpressionPointer LookupChild::evaluate(const Expression* parent) const {
    const auto evaluated_child = child->evaluate(parent);
    return ExpressionPointer{evaluated_child->lookup(name)};
}

ExpressionPointer LookupChild::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, isLetter);
    it = std::find_if_not(it, last, isNameCharacter);
    const auto name = rawString(first, it);

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
