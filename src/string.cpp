#include "string.h"

#include <stdlib.h>

StringView makeStaticString(const char* s) {
    return StringView{s, strlen(s)}; // TODO: should it be +1? No.
}

StringBuilder makeStringBuilder(char* s) {
    auto count = strlen(s);
    return StringBuilder{s, count, count}; // TODO: should it be +1? No.
}

std::string makeStdString(StringBuilder s) {
    return std::string(s.data, s.count);
}

StringBuilder concatenate(StringBuilder base, const char* tail) {
    CONCAT_CSTRING(base, tail);
    return base;
}
