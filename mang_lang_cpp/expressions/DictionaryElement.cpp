#include "DictionaryElement.h"

#include <cassert>

DictionaryElement::DictionaryElement(
    CodeRange range,
    const Expression* parent,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : Expression{range, parent},
    name_{std::move(name)},
    expression{std::move(expression)},
    dictionary_index_{dictionary_index}
{}

std::string DictionaryElement::name() const {return name_->value;}

std::string DictionaryElement::serialize() const {
    if (isWhile()) {
        return "while " + expression->serialize() + ',';
    }
    if (isEnd()) {
        return "end,";
    }
    return name_->serialize() + '=' + expression->serialize() + ',';
}

ExpressionPointer DictionaryElement::evaluate(const Expression*, std::ostream&) const {
    assert(false);
    return nullptr;
}

ExpressionPointer DictionaryElement::lookup(const std::string& s) const {
    if (name_->value == s) {
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

bool DictionaryElement::startsWith(CodeRange) {
    assert(false);
    return false;
}

bool DictionaryElement::isWhile() const {return !name_ && expression;}
bool DictionaryElement::isEnd() const {return !name_ && !expression;}
bool DictionaryElement::isSymbolDefinition() const {return name_ && expression;}
