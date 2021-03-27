#include "Name.h"

std::string Name::serialize() const {
    return value;
}

ExpressionPointer Name::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Name>(begin(), end(), parent, value);
    log << result->serialize() << std::endl;
    return  result;
}

Name Name::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, isLetter);
    code = parseWhile(code, isNameCharacter);
    return Name(first, code.first, nullptr, rawString({first, code.first}));
}

bool Name::startsWith(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    return !code.empty() && std::isalpha(code.begin()->character);
}
