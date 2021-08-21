#include "begin.h"

#include "../factory.h"

const CodeCharacter* begin(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return getCharacter(expression).range.first;
        case CONDITIONAL: return getConditional(expression).range.first;
        case DICTIONARY: return getDictionary(expression).range.first;
        case NAMED_ELEMENT: return getNamedElement(expression).range.first;
        case WHILE_ELEMENT: return getWileElement(expression).range.first;
        case END_ELEMENT: return getEndElement(expression).range.first;
        case FUNCTION: return getFunction(expression).range.first;
        case FUNCTION_DICTIONARY: return getFunctionDictionary(expression).range.first;
        case FUNCTION_LIST: return getFunctionList(expression).range.first;
        case LIST: return getList(expression).range.first;
        case LOOKUP_CHILD: return getLokupChild(expression).range.first;
        case FUNCTION_APPLICATION: return getFunctionApplication(expression).range.first;
        case LOOKUP_SYMBOL: return getLookupSymbol(expression).range.first;
        case NAME: return getName(expression).range.first;
        case NUMBER: return getNumber(expression).range.first;
        case STRING: return getString(expression).range.first;
        default: throw std::runtime_error{"Missing begin operation for : " + std::to_string(expression.type)};
    }
}
