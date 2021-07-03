#include "Name.h"
#include <cassert>

#include "../operations/serialize.h"

std::string Name::serialize() const {
    return ::serialize(*this);
}

NamePointer Name::parse(CodeRange code) {
    auto first = code.begin();
    code = parseWhile(code, isNameCharacter);
    return std::make_shared<Name>(
        CodeRange{first, code.first}, nullptr, rawString({first, code.first})
    );
}

bool Name::startsWith(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    if (code.empty()) {
        return false;
    }
    const auto first = *code.begin();
    return isNameCharacter(first) && !isDigit(first);
}
