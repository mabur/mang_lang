#include "LookupFunction.h"
#include <algorithm>
#include <cassert>

std::string LookupFunction::serialize() const {
    return name.serialize() + " " + child->serialize();
}

ExpressionPointer LookupFunction::evaluate(const Expression* parent, std::ostream& log) const {
    const auto function = parent->lookup(name.value);
    const auto evaluated_child = child->evaluate(parent, log);
    assert(evaluated_child);
    auto result = function->apply(evaluated_child, log);
    log << result->serialize() << std::endl;
    return result;
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
    const auto keywords = std::vector<std::string>{"if", "then", "else", "from", "to"};
    if (isAnyKeyword(first, last, keywords)) {
        return false;
    }
    if (!std::isalpha(first->character)) {
        return false;
    }
    auto it = first;
    it = std::find_if_not(it, last, isNameCharacter);
    it = parseWhiteSpace(it, last);
    return Expression::startsWith(it, last);
}
