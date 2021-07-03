#include "LookupFunction.h"
#include <cassert>

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string LookupFunction::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer LookupFunction::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

ExpressionPointer LookupFunction::parse(CodeRange code) {
    auto first = code.begin();
    auto name = Name::parse(code);
    code.first = name->end();
    const auto expected = ::startsWith(code, '!') ? '!' : '?';
    code = parseCharacter(code, expected);
    auto child = Expression::parse(code);
    code.first = child->end();
    return std::make_shared<LookupFunction>(
        CodeRange{first, code.first}, nullptr, name, std::move(child)
    );
}

bool LookupFunction::startsWith(CodeRange code) {
    if (!Name::startsWith(code)) {
        return false;
    }
    code = parseWhile(code, isNameCharacter);
    return ::startsWith(code, '!') || ::startsWith(code, '?');
}
