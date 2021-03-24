#include "Conditional.h"
#include "../parsing.h"

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

ExpressionPointer Conditional::parse(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    auto expression_if = Expression::parse(code);
    code.first = expression_if->end();
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto expression_then = Expression::parse(code);
    code.first = expression_then->end();
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = Expression::parse(code);
    code.first = expression_else->end();
    code = parseWhiteSpace(code);

    return std::make_shared<Conditional>(
        first,
        code.begin(),
        nullptr,
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    );
}

bool Conditional::startsWith(CodeRange code) {
    return isKeyword(code, "if");
}
