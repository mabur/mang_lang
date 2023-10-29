#include "container.h"

#include <algorithm>

#include "../operations/serialize.h"
#include "../expression.h"
#include "../factory.h"

// Note that we need code range since we use this during parsing.
CodeRange addCodeRanges(Expression rest, Expression top) {
    const auto first_character = std::min(rest.range.first, top.range.first);
    const auto last_character = std::max(rest.range.last, top.range.last);
    return CodeRange{first_character, last_character};
}

Expression putString(Expression rest, Expression top) {
    return makeString(addCodeRanges(top, rest), String{top, rest});
}

Expression putStack(Expression rest, Expression top) {
    return makeStack(addCodeRanges(top, rest), Stack{top, rest});
}

Expression putEvaluatedStack(Expression rest, Expression top) {
    return makeEvaluatedStack(addCodeRanges(top, rest),
        EvaluatedStack{top, rest});
}

Expression reverseString(CodeRange code, Expression string) {
    const auto init = Expression{EMPTY_STRING, 0, code};
    return leftFold(init, string, putString, EMPTY_STRING, getString);
}

Expression reverseStack(CodeRange code, Expression stack) {
    const auto init = Expression{EMPTY_STACK, 0, code};
    return leftFold(init, stack, putStack, EMPTY_STACK, getStack);
}

Expression reverseEvaluatedStack(CodeRange code, Expression stack) {
    const auto init = Expression{EMPTY_STACK, 0, code};
    return leftFold(init, stack, putEvaluatedStack, EMPTY_STACK, getEvaluatedStack);
}

Expression putTable(Expression table, Expression item) {
    const auto tuple = getBinaryTuple(item);
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = storage.evaluated_tables.at(table.index).rows;
    std::string s;
    serialize(s, key);
    rows[s] = {key, value};
    return table;
}

Expression putTableTyped(Expression table, Expression item) {
    if (item.type == ANY) {
        return table;
    }
    const auto tuple = getBinaryTuple(item);
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = getMutableEvaluatedTable(table).rows;
    std::string s;
    serialize_types(s, key);
    rows[s] = {key, value};
    return table;
}

namespace container_functions {

Expression clear(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return Expression{EMPTY_STACK, 0, CodeRange{}};
        case EMPTY_STACK: return Expression{EMPTY_STACK, 0, CodeRange{}};
        case STRING: return Expression{EMPTY_STRING, 0, CodeRange{}};
        case EMPTY_STRING: return Expression{EMPTY_STRING, 0, CodeRange{}};
        case EVALUATED_TABLE: return makeEvaluatedTable(CodeRange{}, EvaluatedTable{});
        case NUMBER: return makeNumber(CodeRange{}, 0);
        case YES: return Expression{NO, 0, CodeRange{}};
        case NO: return in;
        default: throw UnexpectedExpression(in.type, "clear operation");
    }
}

Expression clearTyped(Expression in) {
    // TODO: shouldn't clear EVALUATED_STACK give EMPTY_STACK,
    // so that it can be populated with items of different type?
    //switch (in.type) {
    //    case EVALUATED_STACK: return makeEmptyStack(CodeRange{}, EmptyStack{});
    //    case EMPTY_STACK: return makeEmptyStack(CodeRange{}, EmptyStack{});
    //    case STRING: return makeEmptyString(CodeRange{}, EmptyString{});
    //    case EMPTY_STRING: return makeEmptyString(CodeRange{}, EmptyString{});
    //    case EVALUATED_TABLE: return makeEvaluatedTable(CodeRange{}, EvaluatedTable{});
    //    case NUMBER: return makeNumber(CodeRange{}, 0);
    //    default: throw UnexpectedExpression(in.type, "clear operation");
    //}
    switch (in.type) {
        case EVALUATED_STACK: return in;
        case EMPTY_STACK: return in;
        case STRING: return in;
        case EMPTY_STRING: return in;
        case EVALUATED_TABLE: return in;
        case NUMBER: return in;
        case YES: return in;
        case NO: return in;
        default: throw UnexpectedExpression(in.type, "clearTyped operation");
    }
}

Expression putNumber(Expression collection, Expression item) {
    if (item.type != ANY && item.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens for the operation put!(NUMBER item). " +
                "\nIt expects the item to be a " + NAMES[NUMBER] + "," +
                "\nbut now it got a " + NAMES[item.type] +
                ".\n"
        );
    }
    return makeNumber(
        {}, storage.numbers.at(collection.index) + storage.numbers.at(item.index)
    );
}

Expression putBoolean(Expression, Expression item) {
    return item;
}

Expression put(Expression in) {
    const auto tuple = getBinaryTuple(in);
    const auto item = tuple.left;
    const auto collection = tuple.right;
    switch (collection.type) {
        case EVALUATED_STACK: return putEvaluatedStack(collection, item);
        case EMPTY_STACK: return putEvaluatedStack(collection, item);
        case STRING: return putString(collection, item);
        case EMPTY_STRING: return putString(collection, item);
        case EVALUATED_TABLE: return putTable(collection, item);
        case NUMBER: return putNumber(collection, item);
        case YES: return item;
        case NO: return item;
        default: throw UnexpectedExpression(in.type, "put operation");
    }
}

Expression putTyped(Expression in) {
    const auto tuple = getBinaryTuple(in);
    const auto item = tuple.left;
    const auto collection = tuple.right;
    if (item.type == ANY) {
        return collection;
    }
    switch (collection.type) {
        case EVALUATED_STACK: return putEvaluatedStack(collection, item);
        case EMPTY_STACK: return putEvaluatedStack(collection, item);
        case STRING: return collection; // TODO: type check item
        case EMPTY_STRING: return putString(collection, item);
        case EVALUATED_TABLE: return putTableTyped(collection, item);
        case NUMBER: return putNumber(collection, item);
        case YES: return item; // TODO: type check item
        case NO: return item;// TODO: type check item
        default: throw UnexpectedExpression(in.type, "putTyped operation");
    }
}

template<typename T>
Expression takeTable(const T& table) {
    if (table.empty()) {
        throw std::runtime_error("Cannot take item from empty table");
    }
    const auto& pair = table.begin()->second;
    return makeEvaluatedTuple({}, EvaluatedTuple{{pair.key, pair.value}});
}

template<typename T>
Expression takeTableTyped(const T& table, Expression expression) {
    const auto range = expression.range;
    if (table.empty()) {
        return makeEvaluatedTuple(
            {},
            EvaluatedTuple{{Expression{ANY, 0, range}, Expression{ANY, 0, range}}}
        );
    }
    const auto& pair = table.begin()->second;
    return makeEvaluatedTuple(
        range, EvaluatedTuple{{pair.key, pair.value}}
    );
}

template<typename T>
Expression dropTable(const T& table) {
    return makeEvaluatedTableView({}, EvaluatedTableView{++table.begin(), table.end()});
}

Expression dropNumber(Expression in) {
    return makeNumber({}, storage.numbers.at(in.index) - 1);
}

Expression take(Expression in) {
    const auto type = in.type;
    const auto index = in.index;
    switch (type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.at(index).top;
        case STRING: return storage.strings.at(index).top;
        case EVALUATED_TABLE: return takeTable(storage.evaluated_tables.at(index));
        case EVALUATED_TABLE_VIEW: return takeTable(storage.evaluated_table_views.at(index));
        case NUMBER: return makeNumber({}, 1);
        case YES: return in;
        case NO: return in;
        default: throw UnexpectedExpression(type, "take");
    }
}

Expression takeTyped(Expression in) {
    const auto type = in.type;
    const auto index = in.index;
    switch (type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.at(index).top;
        case STRING: return storage.strings.at(index).top;
        case EVALUATED_TABLE: return takeTableTyped(storage.evaluated_tables.at(index), in);
        case EVALUATED_TABLE_VIEW: return takeTableTyped(storage.evaluated_table_views.at(index), in);
        case EMPTY_STACK: return Expression{ANY, 0, in.range};
        case EMPTY_STRING: return Expression{CHARACTER, 0, in.range};
        case NUMBER: return in;
        case YES: return in;
        case NO: return in;
        default: throw UnexpectedExpression(type, "take");
    }
}

Expression drop(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.at(in.index).rest;
        case STRING: return storage.strings.at(in.index).rest;
        case EVALUATED_TABLE: return dropTable(storage.evaluated_tables.at(in.index));
        case EVALUATED_TABLE_VIEW: return dropTable(storage.evaluated_table_views.at(in.index));
        case EMPTY_STACK: return in;
        case EMPTY_STRING: return in;
        case NUMBER: return dropNumber(in);
        case NO: return in;
        case YES: return Expression{NO, 0, CodeRange{}};
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
        case NUMBER: return in;
        case NO: return in;
        case YES: return in;
        default: throw UnexpectedExpression(in.type,
            "drop typed" + describeLocation(in.range)
        );
    }
}

Expression get(Expression in) {
    if (in.type != EVALUATED_TUPLE) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple of three items," +
                "\nbut now it got a " + NAMES[in.type] +
                ".\n"
        );
    }
    const auto& expressions = storage.evaluated_tuples.at(in.index).expressions;
    if (expressions.size() != 3) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple of three items," +
                "\nbut now it got " + std::to_string(expressions.size()) + "items" +
                ".\n"
        );
    }
    const auto key = expressions.at(0);
    const auto table = expressions.at(1);
    const auto default_value = expressions.at(2);
    if (table.type != EVALUATED_TABLE) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple where the second item is a table," +
                "\nbut now it got a " + NAMES[table.type] +
                ".\n"
        );
    }
    std::string name;
    serialize(name, key);
    const auto& rows = storage.evaluated_tables.at(table.index).rows;
    const auto iterator = rows.find(name);
    return iterator == rows.end() ?
        default_value : iterator->second.value;
}

Expression getTyped(Expression in) {
    if (in.type != EVALUATED_TUPLE) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple of three items," +
                "\nbut now it got a " + NAMES[in.type] +
                ".\n"
        );
    }
    const auto& expressions = storage.evaluated_tuples.at(in.index).expressions;
    if (expressions.size() != 3) {
        throw std::runtime_error(
            std::string{"\n\nI have found a static type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple of three items," +
                "\nbut now it got " + std::to_string(expressions.size()) + "items" +
                ".\n"
        );
    }
    const auto table = expressions.at(1);
    const auto default_value = expressions.at(2);
    if (table.type != EVALUATED_TABLE) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens for the function get!(key table default). " +
                "\nIt expects a tuple where the second item is a table," +
                "\nbut now it got a " + NAMES[table.type] +
                ".\n"
        );
    }
    return default_value;
}

}
