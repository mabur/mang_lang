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
    auto expressions = InternalList{};
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

const InternalList& List::list() const {
    return elements;
}

bool List::boolean() const {
    return !elements.empty();
}

bool List::isEqual(const Expression* expression) const {
    const auto& list_left = list();
    const auto& list_right = expression->list();
    auto left = list().begin();
    auto right = expression->list().begin();
    for (; left != list_left.end() && right != list_right.end(); ++left, ++right) {
        if (!(*left)->isEqual(right->get())) {
            return false;
        }
    }
    return left == list_left.end() && right == list_right.end();
}
