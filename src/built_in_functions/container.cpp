#include "container.h"

#include "binary_tuple.h"
#include "../passes/serialize.h"
#include "../exceptions.h"
#include "../expression.h"
#include "../factory.h"
#include "../string.h"

Expression putString(Expression rest, Expression top) {
    return makeString(rest.range, String{top, rest});
}

Expression putStack(Expression rest, Expression top) {
    return makeStack(rest.range, Stack{top, rest});
}

Expression putEvaluatedStack(Expression rest, Expression top) {
    return makeEvaluatedStack(rest.range, EvaluatedStack{top, rest});
}

Expression putTable(Expression table, Expression item) {
    const auto tuple = getDynamicBinaryTuple(item, "put table");
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = storage.evaluated_tables.at(table.index).rows;
    auto buffer = DynamicString{};
    buffer = serialize(buffer, key);
    auto s = makeStdString(buffer);
    FREE_DARRAY(buffer);
    rows[s] = {key, value};
    return table;
}

Expression putTableTyped(Expression table, Expression item) {
    if (item.type == ANY) {
        return table;
    }
    const auto tuple = getStaticBinaryTuple(item, "putTable");
    const auto key = tuple.left;
    const auto value = tuple.right;
    auto& rows = storage.evaluated_tables.at(table.index).rows;
    auto buffer = DynamicString{};
    buffer = serialize_types(buffer, key);
    auto s = makeStdString(buffer);
    FREE_DARRAY(buffer);
    rows[s] = {key, value};
    return table;
}

namespace container_functions {

Expression clear(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return Expression{0, CodeRange{}, EMPTY_STACK};
        case EMPTY_STACK: return Expression{0, CodeRange{}, EMPTY_STACK};
        case STRING: return Expression{0, CodeRange{}, EMPTY_STRING};
        case EMPTY_STRING: return Expression{0, CodeRange{}, EMPTY_STRING};
        case EVALUATED_TABLE: return makeEvaluatedTable(CodeRange{}, EvaluatedTable{});
        case NUMBER: return makeNumber(CodeRange{}, 0);
        case YES: return Expression{0, CodeRange{}, NO};
        case NO: return in;
        default: throwUnexpectedExpressionException(in.type, "clear operation");
    }
    return Expression{}; // Does not happen
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
    //    default: throwUnexpectedExpressionException(in.type, "clear operation");
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
        default: throwUnexpectedExpressionException(in.type, "clearTyped operation");
    }
    return Expression{}; // Does not happen
}

Expression putNumber(Expression collection, Expression item) {
    if (item.type != ANY && item.type != NUMBER) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens for the operation put!(NUMBER item).\n"
            "It expects the item to be a %s,\n"
            "but now it got a %s.\n",
            getExpressionName(NUMBER),
            getExpressionName(item.type)
        );
    }
    return makeNumber(CodeRange{}, getNumber(collection) + getNumber(item));
}

Expression put(Expression in) {
    const auto tuple = getDynamicBinaryTuple(in, "put");
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
        default: throwUnexpectedExpressionException(in.type, "put operation");
    }
    return Expression{}; // Does not happen
}

Expression putTyped(Expression in) {
    const auto tuple = getStaticBinaryTuple(in, "put");
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
        default: throwUnexpectedExpressionException(in.type, "putTyped operation");
    }
    return Expression{}; // Does not happen
}

template<typename T>
Expression takeTable(const T& table) {
    if (table.empty()) {
        throwException("Cannot take item from empty table");
    }
    const auto& pair = table.begin()->second;
    return makeEvaluatedTuple2(pair.key, pair.value);
}

template<typename T>
Expression takeTableTyped(const T& table, Expression expression) {
    const auto range = expression.range;
    if (table.empty()) {
        return makeEvaluatedTuple2(Expression{0, range, ANY}, Expression{0, range, ANY});
    }
    const auto& pair = table.begin()->second;
    return makeEvaluatedTuple2(pair.key, pair.value);
}

template<typename T>
Expression dropTable(const T& table) {
    return makeEvaluatedTableView(CodeRange{}, EvaluatedTableView{++table.begin(), table.end()});
}

Expression dropNumber(Expression in) {
    return makeNumber(CodeRange{}, getNumber(in) - 1);
}

Expression take(Expression in) {
    const auto type = in.type;
    const auto index = in.index;
    switch (type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.data[index].top;
        case STRING: return storage.strings.data[index].top;
        case EVALUATED_TABLE: return takeTable(storage.evaluated_tables.at(index));
        case EVALUATED_TABLE_VIEW: return takeTable(storage.evaluated_table_views.data[index]);
        case NUMBER: return makeNumber(CodeRange{}, 1);
        case YES: return in;
        case NO: return in;
        default: throwUnexpectedExpressionException(type, "take");
    }
    return Expression{}; // Does not happen
}

Expression takeTyped(Expression in) {
    const auto type = in.type;
    const auto index = in.index;
    switch (type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.data[index].top;
        case STRING: return storage.strings.data[index].top;
        case EVALUATED_TABLE: return takeTableTyped(storage.evaluated_tables.at(index), in);
        case EVALUATED_TABLE_VIEW: return takeTableTyped(storage.evaluated_table_views.data[index], in);
        case EMPTY_STACK: return Expression{0, in.range, ANY};
        case EMPTY_STRING: return Expression{0, in.range, CHARACTER};
        case NUMBER: return in;
        case YES: return in;
        case NO: return in;
        default: throwUnexpectedExpressionException(type, "take");
    }
    return Expression{}; // Does not happen
}

Expression drop(Expression in) {
    switch (in.type) {
        case EVALUATED_STACK: return storage.evaluated_stacks.data[in.index].rest;
        case STRING: return storage.strings.data[in.index].rest;
        case EVALUATED_TABLE: return dropTable(storage.evaluated_tables.at(in.index));
        case EVALUATED_TABLE_VIEW: return dropTable(storage.evaluated_table_views.data[in.index]);
        case EMPTY_STACK: return in;
        case EMPTY_STRING: return in;
        case NUMBER: return dropNumber(in);
        case NO: return in;
        case YES: return Expression{0, CodeRange{}, NO};
        default: throwUnexpectedExpressionException(in.type, "drop");
    }
    return Expression{}; // Does not happen
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
        default: throwUnexpectedExpressionException(in.type, "drop typed");
    }
    return Expression{}; // Does not happen
}

Expression get(Expression in) {
    if (in.type != EVALUATED_TUPLE) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens for the function get!(key table default).\n"
            "It expects a tuple of three items,\n"
            "but now it got a %s.\n",
            getExpressionName(in.type)
        );
    }
    const auto evaluated_tuple = storage.evaluated_tuples.data[in.index];
    const auto count = evaluated_tuple.indices.count;
    if (count != 3) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens for the function get!(key table default).\n"
            "It expects a tuple of three items,\n"
            "but now it got %zu items.\n",
            count
        );
    }
    const auto key = storage.expressions.data[evaluated_tuple.indices.data + 0];
    const auto table = storage.expressions.data[evaluated_tuple.indices.data + 1];
    const auto default_value = storage.expressions.data[evaluated_tuple.indices.data + 2];
    if (table.type != EVALUATED_TABLE) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "It happens for the function get!(key table default).\n"
            "It expects a tuple where the second item is a table,\n"
            "but now it got a %s.\n",
            getExpressionName(table.type)
        );
    }
    auto buffer = DynamicString{};
    buffer = serialize(buffer, key);
    auto name = makeStdString(buffer);
    FREE_DARRAY(buffer);
    const auto& rows = storage.evaluated_tables.at(table.index).rows;
    const auto iterator = rows.find(name);
    return iterator == rows.end() ?
        default_value : iterator->second.value;
}

Expression getTyped(Expression in) {
    if (in.type != EVALUATED_TUPLE) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens for the function get!(key table default).\n"
            "It expects a tuple of three items,\n"
            "but now it got a %s.\n",
            getExpressionName(in.type)
        );
    }
    const auto evaluated_tuple = storage.evaluated_tuples.data[in.index];
    const auto count = evaluated_tuple.indices.count;
    if (count != 3) {
        throwException(
            "\n\nI have found a static type error.\n"
            "It happens for the function get!(key table default).\n"
            "It expects a tuple of three items,\n"
            "but now it got %zu items.\n",
            count
        );
    }
    const auto table = storage.expressions.data[evaluated_tuple.indices.data + 1];
    const auto default_value = storage.expressions.data[evaluated_tuple.indices.data + 2];
    if (table.type != EVALUATED_TABLE) {
        throwException(
            "\n\nI have found a dynamic type error.\n"
            "\nIt happens for the function get!(key table default).\n"
            "It expects a tuple where the second item is a table,\n"
            "but now it got a %s.\n",
            getExpressionName(table.type)
        );
    }
    return default_value;
}

}
