#include "Conditional.h"
#include "../parse_utils.h"

std::string Conditional::serialize() const {
    return std::string{"if "} + expression_if->serialize()
        + " then " + expression_then->serialize()
        + " else " + expression_else->serialize();
}

ExpressionPointer Conditional::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = expression_if->evaluate(parent, log)->boolean() ?
        expression_then->evaluate(parent, log) :
        expression_else->evaluate(parent, log);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Conditional::parse(CodeRange code_range) {
    auto range = code_range;

    range = parseKeyword(range, "if");
    range = parseWhiteSpace(range);
    auto expression_if = Expression::parse(range);
    range = {expression_if->end(), range.end()};
    range = parseWhiteSpace(range);

    range = parseKeyword(range, "then");
    range = parseWhiteSpace(range);
    auto expression_then = Expression::parse(range);
    range = {expression_then->end(), range.end()};
    range = parseWhiteSpace(range);

    range = parseKeyword(range, "else");
    range = parseWhiteSpace(range);
    auto expression_else = Expression::parse(range);
    range = {expression_else->end(), range.end()};
    range = parseWhiteSpace(range);

    return std::make_shared<Conditional>(
        code_range.begin(),
        range.begin(),
        nullptr,
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    );
}

bool Conditional::startsWith(CodeRange code_range) {
    return isKeyword(code_range, "if");
}
