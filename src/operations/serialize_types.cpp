#include "serialize_types.h"

#include "serialize_generic.h"

namespace {

std::string serializeStack(Expression s) {
    return '[' + serialize_types(getStack(s).top) + ']';
}

std::string serializeEvaluatedStack(Expression s) {
    return '[' + serialize_types(getEvaluatedStack(s).top) + ']';
}

} // namespace

std::string serialize_types(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return NAMES[CHARACTER];
        case CONDITIONAL: return serializeConditional(serialize_types, getConditional(expression));
        case IS: return serializeIs(serialize_types, getIs(expression));
        case DICTIONARY: return serializeDictionary(serialize_types, getDictionary(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize_types, getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(serialize_types, getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(serialize_types, getPutAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(serialize_types, getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(serialize_types, getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case FUNCTION: return NAMES[FUNCTION];
        case FUNCTION_BUILT_IN: return "built_in_function";
        case FUNCTION_DICTIONARY: return NAMES[FUNCTION_DICTIONARY];
        case FUNCTION_TUPLE: return NAMES[FUNCTION_TUPLE];
        case TUPLE: return serializeTuple(serialize_types, expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize_types, expression);
        case STACK: return serializeStack(expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case EMPTY_STACK: return NAMES[EMPTY_STACK];
        case LOOKUP_CHILD: return serializeLookupChild(serialize_types, getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(serialize_types, getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case NUMBER: return NAMES[NUMBER];
        case BOOLEAN: return NAMES[BOOLEAN];
        case EMPTY_STRING: return NAMES[EMPTY_STRING];
        case STRING: return NAMES[STRING];
        case ANY: return NAMES[ANY];
        case EMPTY: return NAMES[EMPTY];
        default: throw UnexpectedExpression(expression.type, "serialize types operation");
    }
}
