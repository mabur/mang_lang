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

bool compareDictionaryIndex(
    const DictionaryElementPointer& a, const DictionaryElementPointer& b) {
    return a->dictionary_index_ < b->dictionary_index_;
}

ExpressionPointer Dictionary::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Dictionary>(range(), parent);
    const auto max_element = std::max_element(
        elements.begin(), elements.end(), compareDictionaryIndex
    );
    if (max_element == elements.end()) {
        log << result->serialize() << std::endl;
        return result;
    }
    const auto num_names = 1 + max_element->get()->dictionary_index_;

    result->elements = std::vector<DictionaryElementPointer>(num_names, nullptr);

    for (size_t i = 0; i < elements.size();) {
        const auto& element = elements[i];
        if (element->isSymbolDefinition()) {
            auto evaluated_element = std::make_shared<DictionaryElement>(
                element->range(),
                nullptr,
                element->name_,
                element->expression->evaluate(result.get(), log),
                element->dictionary_index_
            );
            const auto dictionary_index = evaluated_element->dictionary_index_;
            result->elements.at(dictionary_index) = evaluated_element;
            i += element->jump_true;
        }
        if (element->isEnd()) {
            i += element->jump_true;
        }
        if (element->isWhile()) {
            if (element->expression->evaluate(result.get(), log)->boolean()) {
                i += element->jump_true;
            } else {
                i += element->jump_false;
            }
        }
    }
    assert(num_names == result->elements.size());
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
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->isWhile()) {
            while_positions.push_back(i);
        }
        if (element->isEnd()) {
            element->jump_true = while_positions.back() - i;
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", code.begin());
    }
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    for (size_t i = elements.size() - 1; i < elements.size(); --i) {
        auto& element = elements[i];
        if (element->isEnd()) {
            end_positions.push_back(i);
        }
        if (element->isWhile()) {
            element->jump_false = end_positions.back() - i + 1;
            end_positions.pop_back();
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", code.begin());
    }
    // Forward pass to set dictionary_index_:
    auto names = std::vector<std::string>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->isSymbolDefinition()) {
            const auto name = element->name();
            const auto it = std::find(names.begin(), names.end(), name);
            element->dictionary_index_ = std::distance(names.begin(), it);
            if (it == names.end()) {
                names.push_back(name);
            }
        }
    }
    auto result = std::make_shared<Dictionary>(CodeRange{first, code.begin()}, nullptr);
    for (auto& element : elements) {
        result->elements.push_back(std::move(element));
    }
    return result;
}

bool Dictionary::startsWith(CodeRange code) {
    return ::startsWith(code, '{');
}
