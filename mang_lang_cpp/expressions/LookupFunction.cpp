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

ExpressionPointer LookupFunction::parse(CodeRange code) {
    auto first = code.begin();
    auto name = Name::parse(code);
    code.first = name.end();
    code = parseWhiteSpace(code);
    auto child = Expression::parse(code);
    code.first = child->end();
    return std::make_shared<LookupFunction>(
        first, code.first, nullptr, name, std::move(child)
    );
}

bool LookupFunction::startsWith(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    if (!std::isalpha(code.begin()->character)) {
        return false;
    }
    code.first = std::find_if_not(code.begin(), code.end(), isNameCharacter);
    code = parseWhiteSpace(code);
    return Expression::startsWith(code);
}
