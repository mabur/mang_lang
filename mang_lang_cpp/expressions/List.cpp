#include "List.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string List::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer List::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

ExpressionPointer List::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = InternalList{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = Expression::parse(code);
        code.first = expression->end();
        expressions = ::prepend(expressions, std::move(expression));
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return std::make_shared<List>(
        CodeRange{first, code.first}, nullptr, ::reverse(expressions)
    );
}

bool List::startsWith(CodeRange code) {
    return ::startsWith(code, '(');
}
