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

ExpressionPointer LookupChild::parse(CodeRange code) {
    auto first = code.begin();
    auto name = Name::parse(code);
    code.first = name.end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '<');
    code = parseWhiteSpace(code);
    auto child = Expression::parse(code);
    code.first = child->end();
    return std::make_shared<LookupChild>(
        first, code.first, nullptr, name, std::move(child)
    );
}

bool LookupChild::startsWith(CodeRange code) {
    if (code.empty()) {
        return false;
    }
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    if (!std::isalpha(code.begin()->character)) {
        return false;
    }
    code.first = std::find_if_not(code.begin(), code.end(), isNameCharacter);
    code = parseWhiteSpace(code);
    if (code.empty()) {
        return false;
    }
    return code.begin()->character == '<';
}
