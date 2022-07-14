#include "table.h"

#include <algorithm>

#include "../container.h"
#include "../expression.h"
#include "../factory.h"
#include "../operations/serialize.h"

namespace table_functions {

Expression get(Expression in) {
    const auto tuple = getEvaluatedTuple(in);
    const auto name = serialize(tuple.expressions.at(0));
    const auto table = getEvaluatedTable(tuple.expressions.at(1));
    const auto iterator = table.rows.find(name);
    return iterator == table.rows.end() ?
        tuple.expressions.at(2) : iterator->second.value;
}

Expression set(Expression in) {
    const auto tuple = getEvaluatedTuple(in);
    const auto key = tuple.expressions.at(0);
    const auto table = tuple.expressions.at(1);
    const auto value = tuple.expressions.at(2);
    const auto name = serialize(key);
    auto& mutable_table = getMutableEvaluatedTable(table);
    mutable_table.rows[name] = {key, value};
    return table;
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
