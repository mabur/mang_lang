#pragma once

#include "../factory.h"

inline std::string getNameAsLabel(Expression name) {
    return getName(name).value;
}
