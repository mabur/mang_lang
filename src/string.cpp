#include "string.h"

#include <string.h>

#include <carma/carma.h>

StaticString makeStaticString(const char* s) {
    return StaticString{s, strlen(s)};
}

std::string makeStdString(DynamicString s) {
    return std::string(s.data, s.count);
}

DynamicString concatcstring(DynamicString base, const char* tail) {
    CONCAT(base, makeStaticString(tail));
    return base;
}
