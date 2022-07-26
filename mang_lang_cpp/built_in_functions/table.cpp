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

void updateTable(Expression key, Expression value, Expression table) {
    const auto name = serialize(key);
    auto& mutable_table = getMutableEvaluatedTable(table);
    mutable_table.rows[name] = {key, value};
}

Expression put(Expression table, Expression top) {
    const auto tuple = getEvaluatedTuple(top);
    const auto key = tuple.expressions.at(0);
    const auto value = tuple.expressions.at(1);
    updateTable(key, value, table);
    return table;
}

Expression get_keys(Expression in) {
    auto stack = makeEmptyStack({}, {});
    const auto table = getEvaluatedTable(in);
    for (auto it = table.rows.rbegin(); it != table.rows.rend(); ++it) {
        stack = putEvaluatedStack(stack, it->second.key);
    }
    return stack;
}

}
