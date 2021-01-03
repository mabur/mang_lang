#include "LookupFunction.h"
#include <algorithm>
#include <cassert>

std::string LookupFunction::serialize() const {
    return name.serialize() + " " + child->serialize();
}

ExpressionPointer LookupFunction::evaluate(const Expression* parent) const {
    const auto function = parent->lookup(name.value);
    const auto evaluated_child = child->evaluate(parent);
    assert(evaluated_child);
    return function->apply(*evaluated_child);
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
