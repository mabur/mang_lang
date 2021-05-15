#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

DictionaryElement::DictionaryElement(
    CodeRange range,
    const Expression* parent,
    const Name& name,
    ExpressionPointer expression
)
    : Expression{range, parent}, name{name}, expression{std::move(expression)}
{}

std::string DictionaryElement::serialize() const {
    return name.serialize() + '=' + expression->serialize() + ',';
}

ExpressionPointer DictionaryElement::evaluate(
    const Expression* parent, std::ostream& log) const
{
    return std::make_shared<DictionaryElement>(
        range(),
        nullptr,
        name,
        expression->evaluate(parent, log)
    );
}

ExpressionPointer DictionaryElement::lookup(const std::string& s) const {
    if (name.value == s) {
        return expression;
    }
    return nullptr;
}

ExpressionPointer DictionaryElement::parse(CodeRange code) {
    auto first = code.begin();
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    const auto name = Name::parse(code);
    code.first = name.end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = Expression::parse(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    code = parseOptionalCharacter(code, ',');
    return std::make_shared<DictionaryElement>(
        CodeRange{first, code.first}, nullptr, name, std::move(expression)
    );
}

bool DictionaryElement::startsWith(CodeRange) {
    return false;
}
