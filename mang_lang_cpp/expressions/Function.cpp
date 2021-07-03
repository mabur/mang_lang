#include "Function.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"
#include "DictionaryElement.h"

#include "../operations/apply.h"

std::string Function::serialize() const {
    return std::string{"in "} + input_name->serialize() + " out " + body->serialize();
}

ExpressionPointer Function::evaluate(const Expression* environment, std::ostream& log) const {
    auto result = std::make_shared<Function>(range(), environment, input_name, body);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Function::apply(ExpressionPointer input, std::ostream& log) const {
    return ::apply(*this, input, log);
}

ExpressionPointer Function::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    auto input_name = Name::parse(code);
    code.first = input_name->end();
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = Expression::parse(code);
    code.first = body->end();
    return std::make_shared<Function>(
        CodeRange{first, code.begin()}, nullptr, input_name, body
    );
}

bool Function::startsWith(CodeRange code) {
    return isKeyword(code, "in");
}
