#include "starts_with.h"

#include "../parsing.h"

bool startsWithName(CodeRange code) {
    if (code.empty()) {
        return false;
    }
    const auto first = *code.begin();
    return isNameCharacter(first) && !isDigit(first);
}

bool startsWithNumber(CodeRange code) {
    return !code.empty() && (isSign(*code.begin()) || isDigit(*code.begin()));
}
