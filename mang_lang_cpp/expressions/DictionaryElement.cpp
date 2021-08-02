#include "DictionaryElement.h"

#include <cassert>

DictionaryElement::DictionaryElement(
    CodeRange range,
    ExpressionPointer environment,
    ExpressionType type,
    NamePointer name,
    ExpressionPointer expression,
    size_t jump_true,
    size_t jump_false,
    size_t dictionary_index
) : Expression{range, environment, type},
    name{std::move(name)},
    expression{std::move(expression)},
    jump_true_{jump_true},
    jump_false_{jump_false},
    dictionary_index_{dictionary_index}
{}

void setContext(DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            element->jump_true_ = 1; // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            while_positions.push_back(i);
            element->jump_true_ = 1; // dummy
        }
        if (element->type_ == END_ELEMENT) {
            element->jump_true_ = while_positions.back() - i;
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", elements.front()->begin());
    }
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    for (size_t i = elements.size() - 1; i < elements.size(); --i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            element->jump_false_ = 0; // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            element->jump_false_ = end_positions.back() - i + 1;
            end_positions.pop_back();
        }
        if (element->type_ == END_ELEMENT) {
            element->jump_false_ = 0; // dummy
            end_positions.push_back(i);
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", elements.front()->begin());
    }
    // Forward pass to set dictionary_index_:
    auto names = std::vector<std::string>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            const auto name = element->name->value;
            const auto it = std::find(names.begin(), names.end(), name);
            element->dictionary_index_ = std::distance(names.begin(), it);
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (element->type_ == WHILE_ELEMENT) {
            element->dictionary_index_ = 0; // dummy
        }
        if (element->type_ == END_ELEMENT) {
            element->dictionary_index_ = 0; // dummy
        }
    }
}
