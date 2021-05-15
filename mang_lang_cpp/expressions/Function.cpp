#include "Function.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"

std::string Function::serialize() const {
    return std::string{"from "} + input_name.serialize() + " to " + body->serialize();
}

ExpressionPointer Function::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Function>(range(), parent, input_name, body);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer Function::apply(ExpressionPointer input, std::ostream& log) const {
    auto middle = Dictionary({}, parent());
    middle.add(DictionaryElement({}, nullptr, input_name, input));
    auto output = body->evaluate(&middle, log);
    return output;
}

ExpressionPointer Function::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "from");
    code = parseWhiteSpace(code);
    auto input_name = Name::parse(code);
    code.first = input_name.end();
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "to");
    auto body = Expression::parse(code);
    code.first = body->end();
    return std::make_shared<Function>(
        CodeRange{first, code.begin()}, nullptr, input_name, body
    );
}

bool Function::startsWith(CodeRange code) {
    return isKeyword(code, "from ");
}
