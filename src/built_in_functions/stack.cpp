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

Expression clearTyped(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return in;
        case EMPTY_STACK: return in;
        case STRING: return in;
        case EMPTY_STRING: return in;
        case EVALUATED_TABLE: return in;
        default: throw UnexpectedExpression(in.type, "clearTyped operation");
    }
}

Expression put(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto item = binary.left;
    const auto collection = binary.right;
    if (item.type == EMPTY) {
        return collection;
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

Expression putTyped(Expression in) {
    const auto binary = getBinaryInput(in);
    const auto item = binary.left;
    const auto collection = binary.right;
    if (item.type == EMPTY) {
        return collection;
    }
    switch (collection.type) {
        case EVALUATED_STACK: return putEvaluatedStack(collection, item);
        case EMPTY_STACK: return putEvaluatedStack(collection, item);
        case STRING: return collection;
        case EMPTY_STRING: return putString(collection, item);
        case EVALUATED_TABLE: return table_functions::put(collection, item);
        default: throw UnexpectedExpression(in.type, "putTyped operation");
    }
}

template<typename T>
Expression takeTable(const T& table) {
    if (table.empty()) {
        return makeEvaluatedTuple(
            {},
            EvaluatedTuple{{Expression{}, Expression{}}}
        );    
    }
    const auto& pair = table.begin()->second;
    return makeEvaluatedTuple({}, EvaluatedTuple{{pair.key, pair.value}});
}

template<typename T>
Expression dropTable(const T& table) {
    return makeEvaluatedTableView({}, EvaluatedTableView{++table.begin(), table.end()});
}

Expression take(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return getEvaluatedStack(in).top;
        case STRING: return getString(in).top;
        case EVALUATED_TABLE: return takeTable(getEvaluatedTable(in));
        case EVALUATED_TABLE_VIEW: return takeTable(getEvaluatedTableView(in));
        case EMPTY_STACK: return Expression{};
        case EMPTY_STRING: return Expression{CHARACTER, {}, {}};
        default: throw UnexpectedExpression(in.type, "take");
    }
}

Expression drop(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return getEvaluatedStack(in).rest;
        case STRING: return getString(in).rest;
        case EVALUATED_TABLE: return dropTable(getEvaluatedTable(in));
        case EVALUATED_TABLE_VIEW: return dropTable(getEvaluatedTableView(in));
        case EMPTY_STACK: return in;
        case EMPTY_STRING: return in;
        default: throw UnexpectedExpression(in.type, "drop");
    }
}

Expression dropTyped(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return in;
        case STRING: return in;
        case EVALUATED_TABLE: return in;
        case EVALUATED_TABLE_VIEW: return in;
        case EMPTY_STACK: return in;
        case EMPTY_STRING: return in;
        default: throw UnexpectedExpression(in.type, "drop");
    }
}

}
