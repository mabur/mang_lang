#include "Name.h"
#include <cassert>

#include "../operations/serialize.h"

std::string Name::serialize() const {
    return ::serialize(*this);
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
