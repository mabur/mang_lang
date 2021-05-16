#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        auto expression = element->lookup(name);
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
        result += element->serialize();
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
    for (auto element = elements.begin(); element != elements.end(); ++element) {
        result->elements.push_back((*element)->evaluate(result.get(), log));
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
        auto element = ExpressionPointer{};
        if (While::startsWith(code)) {
            element = While::parse(code);
        }
        else if (End::startsWith(code)) {
            element = End::parse(code);
        } else {
            element = DictionaryElement::parse(code);
        }
        code.first = element->end();
        result->elements.push_back(std::move(element));
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

////////////////////////////////////////////////////////////////////////////////

ExpressionPointer DictionaryIteration::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        auto expression = element->lookup(name);
        if (expression) {
            return expression;
        }
    }
    return Expression::lookup(name);
}

std::string DictionaryIteration::serialize() const {
    auto result = std::string{};
    result += '{';
    for (const auto& element : elements) {
        result += element->serialize();
    }
    if (elements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

ExpressionPointer DictionaryIteration::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Dictionary>(range(), parent);
    for (auto element = elements.begin(); element != elements.end(); ++element) {
        result->elements.push_back((*element)->evaluate(result.get(), log));
    }
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer DictionaryIteration::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto result = std::make_shared<DictionaryIteration>(CodeRange{first, code.begin()}, nullptr);
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = ExpressionPointer{};
        if (While::startsWith(code)) {
            element = While::parse(code);
        }
        else if (End::startsWith(code)) {
            element = End::parse(code);
        } else {
            element = DictionaryElement::parse(code);
        }
        code.first = element->end();
        result->elements.push_back(std::move(element));
    }
    code = parseCharacter(code, '}');
    result->range_.last = code.begin();
    return result;
}

bool DictionaryIteration::startsWith(CodeRange code) {
    return ::startsWith(code, '{');
}

bool DictionaryIteration::boolean() const {
    return !elements.empty();
}
