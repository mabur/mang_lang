#include "string.h"

#include <stdlib.h>

#include <carma/carma_string.h>

StringBuilder makeStringBuilder(char* s) {
    return concatenate(StringBuilder{}, s);
}

std::string makeStdString(StringBuilder s) {
    return std::string(s.data, s.count);
}

StringBuilder concatenate(StringBuilder base, const char* tail) {
    CONCAT_CSTRING(base, tail);
    return base;
}
