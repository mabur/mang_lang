#include "DictionaryElement.h"

#include <cassert>

DictionaryElement::DictionaryElement(
    CodeRange range,
    ExpressionPointer environment,
    ExpressionType type,
    NamePointer name,
    ExpressionPointer expression,
    size_t while_index,
    size_t end_index,
    size_t name_index
) : Expression{range, environment, type},
    name{std::move(name)},
    expression{std::move(expression)},
    while_index_{while_index},
    end_index_{end_index},
    name_index_{name_index}
{}

void setContext(DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            element->while_index_ = 1; // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            while_positions.push_back(i);
            element->while_index_ = 1; // dummy
        }
        if (element->type_ == END_ELEMENT) {
            element->while_index_ = while_positions.back();
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
            element->end_index_ = 0; // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            element->end_index_ = end_positions.back();
            end_positions.pop_back();
        }
        if (element->type_ == END_ELEMENT) {
            element->end_index_ = 0; // dummy
            end_positions.push_back(i);
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", elements.front()->begin());
    }
    // Forward pass to set name_index_:
    auto names = std::vector<std::string>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            const auto name = element->name->value;
            const auto it = std::find(names.begin(), names.end(), name);
            element->name_index_ = std::distance(names.begin(), it);
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (element->type_ == WHILE_ELEMENT) {
            element->name_index_ = 0; // dummy
        }
        if (element->type_ == END_ELEMENT) {
            element->name_index_ = 0; // dummy
        }
    }
}
