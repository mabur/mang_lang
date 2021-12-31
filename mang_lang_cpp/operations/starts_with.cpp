#include "starts_with.h"

#include "../parsing.h"

bool startsWithName(CodeRange code) {
    return !code.empty() and isFirstNameCharacter(*code.begin());
}

bool startsWithNumber(CodeRange code) {
    return !code.empty() and isNumeric(*code.begin());
}
