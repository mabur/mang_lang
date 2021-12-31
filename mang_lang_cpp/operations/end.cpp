#include "end.h"

#include "../factory.h"

const CodeCharacter* end(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return getCharacter(expression).range.last;
        case CONDITIONAL: return getConditional(expression).range.last;
        case IS: return getIs(expression).range.last;
        case DICTIONARY: return getDictionary(expression).range.last;
        case DEFINITION: return getDefinition(expression).range.last;
        case WHILE_STATEMENT: return getWileStatement(expression).range.last;
        case END_STATEMENT: return getEndStatement(expression).range.last;
        case FUNCTION: return getFunction(expression).range.last;
        case FUNCTION_BUILT_IN: return getFunctionBuiltIn(expression).range.last;
        case FUNCTION_DICTIONARY: return getFunctionDictionary(expression).range.last;
        case FUNCTION_LIST: return getFunctionList(expression).range.last;
        case LIST: return getList(expression).range.last;
        case EMPTY_LIST: return getEmptyList(expression).range.last;
        case LOOKUP_CHILD: return getLookupChild(expression).range.last;
        case FUNCTION_APPLICATION: return getFunctionApplication(expression).range.last;
        case LOOKUP_SYMBOL: return getLookupSymbol(expression).range.last;
        case NAME: return getName(expression).range.last;
        case NUMBER: return getNumber(expression).range.last;
        case BOOLEAN: return getBoolean(expression).range.last;
        case STRING: return getString(expression).range.last;
        case EMPTY_STRING: return getEmptyString(expression).range.last;
        default: throw UnexpectedExpression(expression.type, "end operation");
    }
}
