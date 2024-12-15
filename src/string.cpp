#include "string.h"

#include <stdlib.h>

#include <carma/carma_string.h>

StringView makeStaticString(const char* s) {
    return STRING_VIEW(s);
}

StringBuilder makeStringBuilder(char* s) {
    auto count = strlen(s);
    // TODO: copy string instead of taking ownership of it.
    return StringBuilder{s, count, count}; // TODO: should it be +1? No.
}

std::string makeStdString(StringBuilder s) {
    return std::string(s.data, s.count);
}

StringBuilder concatenate(StringBuilder base, const char* tail) {
    CONCAT_CSTRING(base, tail);
    return base;
}
