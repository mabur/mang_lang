#include "Name.h"
#include <cassert>

std::string Name::serialize() const {
    return value;
}

NamePointer Name::parse(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, isLetter);
    code = parseWhile(code, isNameCharacter);
    return std::make_shared<Name>(
        CodeRange{first, code.first}, nullptr, rawString({first, code.first})
    );
}

bool Name::startsWith(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    return !code.empty() && std::isalpha(code.begin()->character);
}
