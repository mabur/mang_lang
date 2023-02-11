#include "evaluate_generic.h"

#include "../factory.h"

std::string getNameAsLabel(Expression name) {
    return getName(name).value;
}
