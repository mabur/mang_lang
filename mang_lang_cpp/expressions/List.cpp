#include "List.h"
#include "../algorithm.h"

std::string List::serialize() const {
    if (!elements) {
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

ExpressionPointer List::evaluate(const Expression* parent, std::ostream& log) const {
    const auto operation = [&](const ExpressionPointer& expression) {
        return expression->evaluate(parent, log);
    };
    auto evaluated_elements = map(elements, operation);
    auto result = std::make_shared<List>(begin(), end(), parent, std::move(evaluated_elements));
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer List::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '[');
    it = parseWhiteSpace(it, last);
    auto expressions = InternalList{};
    while (it->character != ']') {
        auto expression = Expression::parse(it, last);
        it = expression->end();
        expressions = prepend(expressions, std::move(expression));
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
    }
    it = parseCharacter(it, ']');
    return std::make_shared<List>(first, it, nullptr, reverse(expressions));
}

bool List::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return first->character == '[';
}

const InternalList& List::list() const {
    return elements;
}

bool List::boolean() const {
    return !!elements;
}

bool List::isEqual(const Expression* expression) const {
    auto left = list();
    auto right = expression->list();
    for (; left && right; left = left->rest, right = right->rest) {
        if (!(left->first)->isEqual(right->first.get())) {
            return false;
        }
    }
    return !left && !right;
}
