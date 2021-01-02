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

ExpressionPointer List::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '[');
    it = parseWhiteSpace(it, last);
    auto expressions = std::vector<ExpressionPointer>{};
    while (it->character != ']') {
        auto expression = Expression::parse(it, last);
        it = expression->end();
        expressions.push_back(std::move(expression));
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
    }
    it = parseCharacter(it, ']');
    return std::make_shared<List>(first, it, nullptr, std::move(expressions));
}
