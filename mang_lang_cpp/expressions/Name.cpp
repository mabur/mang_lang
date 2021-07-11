#include "Name.h"
#include <cassert>

#include "../operations/serialize.h"

std::string Name::serialize() const {
    return ::serialize(this);
}
