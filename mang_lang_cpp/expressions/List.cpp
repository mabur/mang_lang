#include "List.h"
#include "../algorithm.h"

std::string List::serialize() const {
    if (elements.empty()) {
        return "[]";
    }
    const auto operation = [](const std::string& left, const ExpressionPointer& right) {
        return left + right->serialize() + ",";
    };
    auto result = std::string{"["};
    result = leftFold(elements, result, operation);
    result.back() = ']';
    return result;
}

ExpressionPointer List::evaluate(const Expression* parent) const {
    const auto operation = [=](const ExpressionPointer& expression) {
        return expression->evaluate(parent);
    };
    auto evaluated_elements = map(elements, operation);
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

bool List::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return first->character == '[';
}

const std::vector<ExpressionPointer>& List::list() const {
    return elements;
}

bool List::boolean() const {
    return !elements.empty();
}

bool List::isEqual(const Expression* expression) const {
    try {
        const auto& list_left = list();
        const auto& list_right = expression->list();
        if (list_left.size() != list_right.size()) {
            return false;
        }
        for (size_t i = 0; i < list_left.size(); ++i) {
            const auto& left = list_left.at(i);
            const auto& right = list_right.at(i);
            if (!left->isEqual(right.get())) {
                return false;
            }
        }
    } catch (...) {
        return false;
    }
    return true;
}
