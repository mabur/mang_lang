#include "dictionary.h"

#include <algorithm>

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

Expression names(Expression in) {
    const auto dictionary = getEvaluatedDictionary(in);
    auto definitions = dictionary.definitions.sorted();
    std::reverse(definitions.begin(), definitions.end());
    auto list = makeEmptyList({}, {});
    for (const auto& definition : definitions) {
        const auto name = makeName({}, Name{definition.first});
        list = putEvaluatedList(list, name);
    }
    return list;
}

}
