#include "Name.h"
#include <algorithm>

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
    code.first = std::find_if_not(code.begin(), code.end(), isNameCharacter);
    return Name(first, code.first, nullptr, rawString({first, code.first}));
}
