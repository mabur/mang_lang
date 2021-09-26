#include "begin.h"

#include "../factory.h"

const CodeCharacter* begin(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return getCharacter(expression).range.first;
        case CONDITIONAL: return getConditional(expression).range.first;
        case DICTIONARY: return getDictionary(expression).range.first;
        case DEFINITION: return getDefinition(expression).range.first;
        case WHILE_STATEMENT: return getWileStatement(expression).range.first;
        case END_STATEMENT: return getEndStatement(expression).range.first;
        case FUNCTION: return getFunction(expression).range.first;
        case FUNCTION_DICTIONARY: return getFunctionDictionary(expression).range.first;
        case FUNCTION_LIST: return getFunctionList(expression).range.first;
        case LIST: return getList(expression).range.first;
        case EMPTY_LIST: return getEmptyList(expression).range.first;
        case LOOKUP_CHILD: return getLookupChild(expression).range.first;
        case FUNCTION_APPLICATION: return getFunctionApplication(expression).range.first;
        case LOOKUP_SYMBOL: return getLookupSymbol(expression).range.first;
        case NAME: return getName(expression).range.first;
        case NUMBER: return getNumber(expression).range.first;
        case STRING: return getString(expression).range.first;
        case EMPTY_STRING: return getEmptyString(expression).range.first;
        default: throw std::runtime_error{"Missing begin operation for : " + std::to_string(expression.type)};
    }
}
