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
    const auto table = getEvaluatedTable(binary.right);
    const auto iterator = table.rows.find(name);
    if (iterator == table.rows.end()) {
        throw MissingSymbol(name, "get");
    }
    const auto result = iterator->second.value;
    return result;
}

Expression get_names(Expression in) {
    const auto dictionary = getEvaluatedDictionary(in);
    auto definitions = dictionary.definitions.sorted();
    std::reverse(definitions.begin(), definitions.end());
    auto stack = makeEmptyStack({}, {});
    for (const auto& definition : definitions) {
        const auto name = makeName({}, Name{definition.first});
        stack = putEvaluatedStack(stack, name);
    }
    return stack;
}

}
