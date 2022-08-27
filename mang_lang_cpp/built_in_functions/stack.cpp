#include "stack.h"

#include "../factory.h"
#include "../container.h"

#include "table.h"

namespace stack_functions {

Expression clear(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return makeEmptyStack(CodeRange{}, EmptyStack{});
        case EMPTY_STACK: return makeEmptyStack(CodeRange{}, EmptyStack{});
        case STRING: return makeEmptyString(CodeRange{}, EmptyString{});
        case EMPTY_STRING: return makeEmptyString(CodeRange{}, EmptyString{});
        case EVALUATED_TABLE: return makeEvaluatedTable(CodeRange{}, EvaluatedTable{});
        default: throw UnexpectedExpression(in.type, "clear operation");
    }
}

Expression put(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto item = binary.left;
    const auto collection = binary.right;
    if (item.type == EMPTY) {
        return binary.right;
    }
    switch (collection.type) {
        case EVALUATED_STACK: return putEvaluatedStack(collection, item);
        case EMPTY_STACK: return putEvaluatedStack(collection, item);
        case STRING: return putString(collection, item);
        case EMPTY_STRING: return putString(collection, item);
        case EVALUATED_TABLE: return table_functions::put(collection, item);
        default: throw UnexpectedExpression(in.type, "put operation");
    }
}

Expression takeEvaluatedTable(const EvaluatedTable& table) {
    auto first = table.rows.begin();
    return makeEvaluatedTuple(
        {}, EvaluatedTuple{{first->second.key, first->second.value}}
    );
}

Expression takeEvaluatedTableView(const EvaluatedTableView& table) {
    auto first = table.first;
    return makeEvaluatedTuple(
        {}, EvaluatedTuple{{first->second.key, first->second.value}}
    );
}

Expression dropEvaluatedTable(const EvaluatedTable& table) {
    auto first = table.rows.begin();
    auto last = table.rows.end();
    return makeEvaluatedTableView({}, EvaluatedTableView{++first, last});
}

Expression dropEvaluatedTableView(const EvaluatedTableView& table) {
    auto first = table.first;
    auto last = table.last;
    return makeEvaluatedTableView({}, EvaluatedTableView{++first, last});
}

Expression take(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return getEvaluatedStack(in).top;
        case STRING: return getString(in).top;
        case EVALUATED_TABLE: return takeEvaluatedTable(getEvaluatedTable(in));
        case EVALUATED_TABLE_VIEW: return takeEvaluatedTableView(getEvaluatedTableView(in));
        case EMPTY_STACK: return Expression{ANY, {}, {}};
        case EMPTY_STRING: return Expression{ANY, {}, {}};
        default: throw UnexpectedExpression(in.type, "take");
    }
}

Expression drop(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return getEvaluatedStack(in).rest;
        case STRING: return getString(in).rest;
        case EVALUATED_TABLE: return dropEvaluatedTable(getEvaluatedTable(in));
        case EVALUATED_TABLE_VIEW: return dropEvaluatedTableView(getEvaluatedTableView(in));
        case EMPTY_STACK: return Expression{ANY, {}, {}};
        case EMPTY_STRING: return Expression{ANY, {}, {}};
        default: throw UnexpectedExpression(in.type, "drop");
    }
}

}
