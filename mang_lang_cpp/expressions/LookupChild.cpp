#include "LookupChild.h"

std::string LookupChild::serialize() const {
    return name.serialize() + "@" + child->serialize();
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
    code = parseCharacter(code, '@');
    code = parseWhiteSpace(code);
    auto child = Expression::parse(code);
    code.first = child->end();
    return std::make_shared<LookupChild>(
        CodeRange{first, code.first}, nullptr, name, std::move(child)
    );
}

bool LookupChild::startsWith(CodeRange code) {
    if (!Name::startsWith(code)) {
        return false;
    }
    code = parseWhile(code, isNameCharacter);
    code = parseWhiteSpace(code);
    return ::startsWith(code, '@');
}
