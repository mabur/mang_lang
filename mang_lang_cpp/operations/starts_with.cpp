#include "starts_with.h"

#include <vector>

#include "../parsing.h"

const auto KEYWORDS = std::vector<std::string>{
    "in", "out", "if", "then", "else", "is", "while", "end"
};

bool startsWithName(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    if (code.empty()) {
        return false;
    }
    const auto first = *code.begin();
    return isNameCharacter(first) && !isDigit(first);
}

bool startsWithNumber(CodeRange code) {
    return !code.empty() && (isSign(*code.begin()) || isDigit(*code.begin()));
}
