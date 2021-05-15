#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>

void Dictionary::add(DictionaryElement element) {
    elements.push_back(std::move(element));
}

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        auto expression = element.lookup(name);
        if (expression) {
            return expression;
        }
    }
    return Expression::lookup(name);
}

std::string Dictionary::serialize() const {
    auto result = std::string{};
    result += '{';
    for (const auto& element : elements) {
        result += element.serialize();
    }
    if (elements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

ExpressionPointer Dictionary::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Dictionary>(range(), parent);
    for (const auto& element : elements) {
        result->add(DictionaryElement{
            {},
            nullptr,
            element.name,
            element.expression->evaluate(result.get(), log)
        });
    }
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Dictionary::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto result = std::make_shared<Dictionary>(CodeRange{first, code.begin()}, nullptr);
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = DictionaryElement::parse(code);
        code.first = element.end();
        result->add(std::move(element));
    }
    code = parseCharacter(code, '}');
    result->range_.last = code.begin();
    return result;
}

bool Dictionary::startsWith(CodeRange code) {
    return ::startsWith(code, '{');
}

bool Dictionary::boolean() const {
    return !elements.empty();
}
