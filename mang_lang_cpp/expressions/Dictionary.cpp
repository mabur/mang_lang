#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>
#include <cassert>

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

ExpressionPointer Dictionary::parse(CodeRange) {
    assert(false);
    return nullptr;
}

bool Dictionary::startsWith(CodeRange) {
    return false;
}

bool Dictionary::boolean() const {
    return !elements.empty();
}

////////////////////////////////////////////////////////////////////////////////

ExpressionPointer DictionaryIteration::lookup(const std::string&) const {
    assert(false);
    return nullptr;
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
    auto while_positions = std::vector<size_t>{};
    auto result = std::make_shared<DictionaryIteration>(CodeRange{first, code.begin()}, nullptr);
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = DictionaryElementBasePointer{};
        if (While::startsWith(code)) {
            element = While::parse(code);
            while_positions.push_back(result->elements.size());
        }
        else if (End::startsWith(code)) {
            element = End::parse(code);
            assert(!while_positions.empty()); // TODO: parse error message
            const auto while_position = while_positions.back();
            const auto end_position = result->elements.size();
            while_positions.pop_back();
            element->index = while_position;
            result->elements[while_position]->index = end_position;
        } else {
            element = DictionaryElement::parse(code);
            if (!while_positions.empty()) {
                // TODO: find key
                const auto is_same_name = [&](const DictionaryElementBasePointer& x) -> bool {
                    return x->name() == element->name();
                };
                const auto it = std::find_if(
                    result->elements.begin(), result->elements.end(), is_same_name
                );
                assert(it != result->elements.end()); // TODO: parse error message
                element->index = std::distance(result->elements.begin(), it);
            }
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
    assert(false);
    return false;
}
