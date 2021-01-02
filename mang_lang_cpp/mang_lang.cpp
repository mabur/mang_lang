#include "mang_lang.h"
#include "expressions/Expression.h"

ExpressionPointer parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return Expression::parse(result.data(), result.data() + result.size());
}

std::string roundtrip(std::string code) {
    return parse(code)->serialize();
}

std::string evaluate(std::string code) {
    return parse(code)->evaluate(nullptr)->serialize();
}
