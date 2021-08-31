#include "end.h"

#include "../factory.h"

const CodeCharacter* end(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return getCharacter(expression).range.last;
        case CONDITIONAL: return getConditional(expression).range.last;
        case DICTIONARY: return getDictionary(expression).range.last;
        case DEFINITION: return getDefinition(expression).range.last;
        case WHILE_STATEMENT: return getWileStatement(expression).range.last;
        case END_ELEMENT: return getEndElement(expression).range.last;
        case FUNCTION: return getFunction(expression).range.last;
        case FUNCTION_DICTIONARY: return getFunctionDictionary(expression).range.last;
        case FUNCTION_LIST: return getFunctionList(expression).range.last;
        case LIST: return getList(expression).range.last;
        case LOOKUP_CHILD: return getLokupChild(expression).range.last;
        case FUNCTION_APPLICATION: return getFunctionApplication(expression).range.last;
        case LOOKUP_SYMBOL: return getLookupSymbol(expression).range.last;
        case NAME: return getName(expression).range.last;
        case NUMBER: return getNumber(expression).range.last;
        case NEW_STRING: return getNewString(expression).range.last;
        case NEW_EMPTY_STRING: return getNewEmptyString(expression).range.last;
        default: throw std::runtime_error{"Missing end operation for : " + std::to_string(expression.type)};
    }
}
