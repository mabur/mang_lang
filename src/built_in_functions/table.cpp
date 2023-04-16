#include "table.h"

#include <algorithm>

#include "../algorithm.h"
#include "../expression.h"
#include "../factory.h"
#include "../operations/serialize.h"

namespace table_functions {

Expression get(Expression in) {
    const auto tuple = getEvaluatedTuple(in);
    const auto key = tuple.expressions.at(0);
    const auto table = tuple.expressions.at(1);
    const auto name = serialize(key);
    const auto& rows = getEvaluatedTable(table).rows;
    const auto iterator = rows.find(name);
    return iterator == rows.end() ?
        tuple.expressions.at(2) : iterator->second.value;
}

Expression put(Expression table, Expression item) {
    const auto tuple = getBinaryTuple(item);
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = getMutableEvaluatedTable(table).rows;
    rows[serialize(key)] = {key, value};
    return table;
}

Expression putTyped(Expression table, Expression item) {
    if (item.type == ANY) {
        return table;
    }
    const auto tuple = getBinaryTuple(item);
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = getMutableEvaluatedTable(table).rows; 
    rows[serialize_types(key)] = {key, value};
    return table;
}

}
