#include "LookupFunction.h"
#include <algorithm>

std::string LookupFunction::serialize() const {
    return name.serialize() + " " + child->serialize();
}

ExpressionPointer LookupFunction::evaluate(const Expression* parent) const {
    // TODO: implement function application.
    const auto evaluated_child = child->evaluate(parent);
    return ExpressionPointer{evaluated_child->lookup(name.value)};
}

ExpressionPointer LookupFunction::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto name = Name::parse(first, last);
    auto it = name.end();
    it = parseWhiteSpace(it, last);
    auto child = Expression::parse(it, last);
    it = child->end();
    return std::make_shared<LookupFunction>(first, it, nullptr, name, std::move(child));
}

bool LookupFunction::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    if (!std::isalpha(first->character)) {
        return false;
    }
    auto it = first;
    it = std::find_if_not(it, last, isNameCharacter);
    it = parseWhiteSpace(it, last);
    return Expression::startsWith(it, last);
}
