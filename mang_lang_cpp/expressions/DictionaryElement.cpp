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

std::vector<size_t> whileIndices(DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    auto while_indices = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            while_indices.push_back(1); // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            while_positions.push_back(i);
            while_indices.push_back(1); // dummy
        }
        if (element->type_ == END_ELEMENT) {
            while_indices.push_back(while_positions.back());
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", elements.front()->begin());
    }
    return while_indices;
}

std::vector<size_t> endIndices(DictionaryElements& elements) {
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    auto end_indices = std::vector<size_t>(elements.size());
    for (size_t i = elements.size() - 1; i < elements.size(); --i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            end_indices[i] = 0; // dummy
        }
        if (element->type_ == WHILE_ELEMENT) {
            end_indices[i] = end_positions.back();
            end_positions.pop_back();
        }
        if (element->type_ == END_ELEMENT) {
            end_indices[i] = 0; // dummy
            end_positions.push_back(i);
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", elements.front()->begin());
    }
    return end_indices;
}

std::vector<size_t> nameIndices(DictionaryElements& elements) {
    // Forward pass to set name_index_:
    auto names = std::vector<std::string>{};
    auto name_indices = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->type_ == NAMED_ELEMENT) {
            const auto name = element->name->value;
            const auto it = std::find(names.begin(), names.end(), name);
            name_indices.push_back(std::distance(names.begin(), it));
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (element->type_ == WHILE_ELEMENT) {
            name_indices.push_back(0); // dummy
        }
        if (element->type_ == END_ELEMENT) {
            name_indices.push_back(0); // dummy
        }
    }
    return name_indices;
}

void setContext(DictionaryElements& elements) {
    const auto while_indices = whileIndices(elements);
    const auto end_indices = endIndices(elements);
    const auto name_indices = nameIndices(elements);
    for (size_t i = 0; i < elements.size(); ++i) {
        elements[i]->name_index_ = name_indices[i];
        elements[i]->while_index_ = while_indices[i];
        elements[i]->end_index_ = end_indices[i];
    }
}
