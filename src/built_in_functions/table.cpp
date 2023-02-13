#include "table.h"

#include <algorithm>

#include "../container.h"
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

void updateTable(Expression key, Expression value, Expression table) {
    getMutableEvaluatedTable(table).rows[serialize(key)] = {key, value};
}

Expression put(Expression table, Expression item) {
    try {
        const auto tuple = getEvaluatedTuple(item);
        const auto key = tuple.expressions.at(0);
        const auto value = tuple.expressions.at(1);
        updateTable(key, value, table);
        return table;
    }
    catch (std::runtime_error&) {
        const auto rows = std::map<std::string, Row>{
            {NAMES[ANY], {Expression{ANY, {},{}}, Expression{ANY, {}, {}}}}
        };
        return makeEvaluatedTable(
            {},
            EvaluatedTable{rows}
        );
    }
}

}
