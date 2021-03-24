#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>

void Dictionary::add(DictionaryElement element) {
    elements.push_back(std::move(element));
}

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        if (element.name.value == name) {
            return element.expression;
        }
    }
    return Expression::lookup(name);
}

std::string Dictionary::serialize() const {
    auto result = std::string{};
    result += '{';
    for (const auto& element : elements) {
        result += element.name.serialize();
        result += '=';
        result += element.expression->serialize();
        result += ',';
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
    auto result = std::make_shared<Dictionary>(
        begin(), end(), parent);
    for (const auto& element : elements) {
        result->add(DictionaryElement{
            element.name,
            element.expression->evaluate(result.get(), log)
        });
    }
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Dictionary::parse(CodeRange code_range) {
    auto range = code_range;
    range = parseCharacter(range, '{');
    range = parseWhiteSpace(range);
    auto result = std::make_shared<Dictionary>(code_range.begin(), range.begin(), nullptr);
    while (range.begin()->character != '}') {
        const auto name = Name::parse(range);
        range = {name.end(), code_range.end()};
        range = parseWhiteSpace(range);
        range = parseCharacter(range, '=');
        range = parseWhiteSpace(range);
        auto expression = Expression::parse(range);
        range = {expression->end(), code_range.end()};
        result->add(DictionaryElement{name, std::move(expression)});
        range = parseWhiteSpace(range);
        range = parseOptionalCharacter(range, ',');
        range = parseWhiteSpace(range);
    }
    range = parseCharacter(range, '}');
    result->last_ = range.begin();
    return result;
}

bool Dictionary::startsWith(CodeRange code) {
    return !code.empty() && code.begin()->character == '{';
}

bool Dictionary::boolean() const {
    return !elements.empty();
}
