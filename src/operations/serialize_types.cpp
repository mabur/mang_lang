#include "serialize_types.h"

#include "serialize_generic.h"
#include "../container.h"

namespace {

std::string serializeEvaluatedStack(Expression s) {
    return '[' + serialize_types(getEvaluatedStack(s).top) + ']';
}

std::string serializeTable(Expression s) {
    const auto rows = getTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    const auto& row = *rows.begin();
    return '<' + serialize_types(row.key) + ':' + serialize_types(row.value) + '>';
}

std::string serializeEvaluatedTable(Expression s) {
    const auto& rows = getEvaluatedTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    const auto& row = rows.begin()->second;
    return '<' + serialize_types(row.key) + ':' + serialize_types(row.value) + '>';
}

} // namespace

std::string serialize_types(Expression expression) {
    switch (expression.type) {
        case CONDITIONAL: return serializeConditional(serialize_types, getConditional(expression));
        case IS: return serializeIs(serialize_types, getIs(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize_types, getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(serialize_types, getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(serialize_types, getPutAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(serialize_types, getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(serialize_types, getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case TABLE: return serializeTable(expression);
        case EVALUATED_TABLE: return serializeEvaluatedTable(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize_types, expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case LOOKUP_CHILD: return serializeLookupChild(serialize_types, getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(serialize_types, getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case DYNAMIC_EXPRESSION: return serializeDynamicExpression(serialize_types, getDynamicExpression(expression));
        default: return NAMES[expression.type];
    }
}
