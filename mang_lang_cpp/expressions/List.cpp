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

ExpressionPointer List::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '[');
    code = parseWhiteSpace(code);
    auto expressions = InternalList{};
    while (!::startsWith(code, ']')) {
        verifyThisIsNotTheEnd(code);
        auto expression = Expression::parse(code);
        code.first = expression->end();
        expressions = prepend(expressions, std::move(expression));
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
    }
    code = parseCharacter(code, ']');
    return std::make_shared<List>(first, code.first, nullptr, reverse(expressions));
}

bool List::startsWith(CodeRange code) {
    return ::startsWith(code, '[');
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
