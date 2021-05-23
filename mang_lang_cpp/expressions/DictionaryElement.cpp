#include "DictionaryElement.h"

#include <cassert>

DictionaryElement::DictionaryElement(
    CodeRange range,
    const Expression* parent,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : Expression{range, parent},
    name{std::move(name)},
    expression{std::move(expression)},
    dictionary_index_{dictionary_index}
{}

std::string DictionaryElement::serialize() const {
    if (isWhile()) {
        return "while " + expression->serialize() + ',';
    }
    if (isEnd()) {
        return "end,";
    }
    return name->serialize() + '=' + expression->serialize() + ',';
}

ExpressionPointer DictionaryElement::lookup(const std::string& s) const {
    if (name->value == s) {
        return expression;
    }
    return nullptr;
}

DictionaryElementPointer DictionaryElement::parse(CodeRange code) {
    auto first = code.begin();
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (isKeyword(code, "end")) {
        code = parseKeyword(code, "end");
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        return std::make_shared<DictionaryElement>(
            CodeRange{first, code.first}, nullptr, nullptr, nullptr, 0
        );
    }
    if (isKeyword(code, "while")) {
        code = parseKeyword(code, "while");
        code = parseWhiteSpace(code);
        auto expression = Expression::parse(code);
        code.first = expression->end();
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        return std::make_shared<DictionaryElement>(
            CodeRange{first, code.first}, nullptr, nullptr, std::move(expression), 0
        );
    }
    auto name = Name::parse(code);
    code.first = name->end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = Expression::parse(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    code = parseOptionalCharacter(code, ',');
    return std::make_shared<DictionaryElement>(
        CodeRange{first, code.first}, nullptr, std::move(name), std::move(expression), 0
    );
}

bool DictionaryElement::isWhile() const {return !name && expression;}
bool DictionaryElement::isEnd() const {return !name && !expression;}
bool DictionaryElement::isSymbolDefinition() const {return name && expression;}
