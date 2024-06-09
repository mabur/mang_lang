#include "string.h"

std::string makeStdString(SerializedString s) {
    return std::string(s.data, s.count);
}
