#include "Function.h"
#include <algorithm>
#include "../parse_utils.h"
#include "Dictionary.h"

std::string Function::serialize() const {
    return std::string{"from "} + input_name.serialize() + " to " + body->serialize();
}

ExpressionPointer Function::evaluate(const Expression* parent) const {
    return std::make_shared<Function>(begin(), end(), parent, input_name, body);
}

ExpressionPointer Function::apply(ExpressionPointer input) const {
    auto middle = Dictionary(nullptr, nullptr, parent());
    middle.add(DictionaryElement(input_name, input));
    auto output = body->evaluate(&middle);
    return output;
}

ExpressionPointer Function::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseKeyword(it, "from");
    it = parseWhiteSpace(it, last);
    auto input_name = Name::parse(it, last);
    it = input_name.end();
    it = parseWhiteSpace(it, last);
    it = parseKeyword(it, "to");
    auto body = Expression::parse(it, last);
    it = body->end();
    return std::make_shared<Function>(first, it, nullptr, input_name, body);
}

bool Function::startsWith(const CodeCharacter* first, const CodeCharacter*) {
    return isKeyword(first, "from ");
}
