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
        case EVALUATED_TABLE: return table_functions::putTable(collection, item);
        default: throw UnexpectedExpression(in.type, "put operation");
    }
}

}
