#include "string.h"

ConstantString makeStaticString(const char* s) {
    return ConstantString{s, strlen(s)};
}

std::string makeStdString(DynamicString s) {
    return std::string(s.data, s.count);
}

DynamicString concatenate(DynamicString base, const char* tail) {
    CONCAT(base, makeStaticString(tail));
    return base;
}
