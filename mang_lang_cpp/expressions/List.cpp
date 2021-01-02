#include "List.h"

std::string List::serialize() const {
    auto result = std::string{};
    result += '[';
    for (const auto& element : elements) {
        result += element->serialize();
        result += ',';
    }
    if (elements.empty()) {
        result += ']';
    }
    else {
        result.back() = ']';
    }
    return result;
}

ExpressionPointer List::evaluate(const Expression* parent) const {
    auto evaluated_elements = std::vector<ExpressionPointer>{};
    for (const auto& element : elements) {
        evaluated_elements.emplace_back(element->evaluate(parent));
    }
    return std::make_shared<List>(begin(), end(), parent, std::move(evaluated_elements));
}
