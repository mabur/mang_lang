#include "Conditional.h"
#include "../parse_utils.h"

std::string Conditional::serialize() const {
    return std::string{"if "} + expression_if->serialize()
        + " then " + expression_then->serialize()
        + " else " + expression_else->serialize();
}

ExpressionPointer Conditional::evaluate(const Expression* parent) const {
    if (expression_if->evaluate(parent)->boolean()) {
        return expression_then->evaluate(parent);
    } else {
        return expression_else->evaluate(parent);
    }
}

ExpressionPointer Conditional::parse(
    const CodeCharacter* first, const CodeCharacter* last
) {
    auto it = first;

    it = parseKeyword(it, "if");
    it = parseWhiteSpace(it, last);
    auto expression_if = Expression::parse(it, last);
    it = expression_if->end();
    it = parseWhiteSpace(it, last);

    it = parseKeyword(it, "then");
    it = parseWhiteSpace(it, last);
    auto expression_then = Expression::parse(it, last);
    it = expression_then->end();
    it = parseWhiteSpace(it, last);

    it = parseKeyword(it, "else");
    it = parseWhiteSpace(it, last);
    auto expression_else = Expression::parse(it, last);
    it = expression_else->end();
    it = parseWhiteSpace(it, last);

    return std::make_shared<Conditional>(
        first,
        it,
        nullptr,
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    );
}

bool Conditional::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    return isKeyword(first, "if");
}
