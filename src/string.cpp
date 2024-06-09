#include "string.h"

#include <string.h>

StaticString makeStaticString(const char* s) {
    return StaticString{s, strlen(s)};
}

std::string makeStdString(DynamicString s) {
    return std::string(s.data, s.count);
}
