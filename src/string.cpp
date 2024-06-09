#include "string.h"

std::string makeStdString(DynamicString s) {
    return std::string(s.data, s.count);
}
