#include "dictionary.h"

#include "../container.h"
#include "../expression.h"
#include "../factory.h"
#include "../operations/serialize.h"

namespace dictionary_functions {

Expression get(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto name = serialize(binary.left);
    const auto dictionary = getEvaluatedDictionary(binary.right);
    const auto result = dictionary.definitions.lookup(name);
    if (result.type != EMPTY) {
        return result;
    }
    throw MissingSymbol(name, "dictionary");
}

}
