#include "LookupChild.h"
#include <algorithm>

std::string LookupChild::serialize() const {
    return name.serialize() + "<" + child->serialize();
}

ExpressionPointer LookupChild::evaluate(const Expression* parent, std::ostream& log) const {
    const auto evaluated_child = child->evaluate(parent, log);
    auto result = ExpressionPointer{evaluated_child->lookup(name.value)};
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer LookupChild::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto name = Name::parse(first, last);
    auto it = name.end();
    it = parseWhiteSpace(it, last);
    it = parseCharacter(it, last, '<');
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
    it = parseWhiteSpace(it, last);
    return it->character == '<';
}
