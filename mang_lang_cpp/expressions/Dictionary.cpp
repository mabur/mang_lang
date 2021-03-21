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

ExpressionPointer Dictionary::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, last, '{');
    it = parseWhiteSpace(it, last);
    auto result = std::make_shared<Dictionary>(first, it, nullptr);
    while (it->character != '}') {
        const auto name = Name::parse(it, last);
        it = name.end();
        it = parseWhiteSpace(it, last);
        it = parseCharacter(it, last, '=');
        it = parseWhiteSpace(it, last);
        auto expression = Expression::parse(it, last);
        it = expression->end();
        result->add(DictionaryElement{name, std::move(expression)});
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
        it = parseWhiteSpace(it, last);
    }
    it = parseCharacter(it, last, '}');
    result->last_ = it;
    return result;
}

bool Dictionary::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return first->character == '{';
}

bool Dictionary::boolean() const {
    return !elements.empty();
}
