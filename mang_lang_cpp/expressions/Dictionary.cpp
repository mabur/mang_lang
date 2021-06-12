#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>
#include <cassert>
#include <iostream>

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        if (element) {
            auto expression = element->lookup(name);
            if (expression) {
                return expression;
            }
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

ExpressionPointer Dictionary::evaluate(const Expression* environment, std::ostream& log) const {
    const auto num_names = numNames(elements);
    auto result = std::make_shared<Dictionary>(range(), environment);
    result->elements = std::vector<DictionaryElementPointer>(num_names, nullptr);
    auto i = size_t{0};
    while (i < elements.size()) {
        elements[i]->mutate(result.get(), log, result->elements);
        i += elements[i]->jump(result.get(), log);
    }
    log << result->serialize() << std::endl;
    return result;
}

bool Dictionary::boolean() const {
    return !elements.empty();
}

ExpressionPointer Dictionary::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto elements = std::vector<DictionaryElementPointer>{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = DictionaryElement::parse(code);
        code.first = element->end();
        elements.push_back(element);
    }
    code = parseCharacter(code, '}');
    setContext(elements);
    auto result = std::make_shared<Dictionary>(CodeRange{first, code.begin()}, nullptr);
    for (auto& element : elements) {
        result->elements.push_back(std::move(element));
    }
    return result;
}

bool Dictionary::startsWith(CodeRange code) {
    return ::startsWith(code, '{');
}
