#include "expression.h"
#include <string.h>

#include "exceptions.h"
#include "factory.h"

const char* getExpressionName(ExpressionType type) {
    switch (type) {
        case CHARACTER: return "CHARACTER";
        case CONDITIONAL: return "CONDITIONAL";
        case IS: return "IS";
        case ALTERNATIVE: return "ALTERNATIVE";
        case TABLE: return "TABLE";
        case EVALUATED_TABLE: return "EVALUATED_TABLE";
        case EVALUATED_TABLE_VIEW: return "EVALUATED_TABLE_VIEW";
        case DICTIONARY: return "DICTIONARY";
        case EVALUATED_DICTIONARY: return "EVALUATED_DICTIONARY";
        case TUPLE: return "TUPLE";
        case EVALUATED_TUPLE: return "EVALUATED_TUPLE";
        case FUNCTION: return "FUNCTION";
        case FUNCTION_BUILT_IN: return "FUNCTION_BUILT_IN";
        case FUNCTION_DICTIONARY: return "FUNCTION_DICTIONARY";
        case FUNCTION_TUPLE: return "FUNCTION_TUPLE";
        case STACK: return "STACK";
        case EVALUATED_STACK: return "EVALUATED_STACK";
        case EMPTY_STACK: return "EMPTY_STACK";
        case LOOKUP_CHILD: return "LOOKUP_CHILD";
        case FUNCTION_APPLICATION: return "FUNCTION_APPLICATION";
        case LOOKUP_SYMBOL: return "LOOKUP_SYMBOL";
        case NAME: return "NAME";
        case ARGUMENT: return "ARGUMENT";
        case DEFINITION: return "DEFINITION";
        case PUT_ASSIGNMENT: return "PUT_ASSIGNMENT";
        case PUT_EACH_ASSIGNMENT: return "PUT_EACH_ASSIGNMENT";
        case DROP_ASSIGNMENT: return "DROP_ASSIGNMENT";
        case WHILE_STATEMENT: return "WHILE_STATEMENT";
        case FOR_STATEMENT: return "FOR_STATEMENT";
        case FOR_SIMPLE_STATEMENT: return "FOR_SIMPLE_STATEMENT";
        case WHILE_END_STATEMENT: return "WHILE_END_STATEMENT";
        case FOR_END_STATEMENT: return "FOR_END_STATEMENT";
        case FOR_SIMPLE_END_STATEMENT: return "FOR_SIMPLE_END_STATEMENT";
        case RETURN_STATEMENT: return "RETURN_STATEMENT";
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case EMPTY_STRING: return "EMPTY_STRING";
        case YES: return "YES";
        case NO: return "NO";
        case DYNAMIC_EXPRESSION: return "DYNAMIC_EXPRESSION";
        case TYPED_EXPRESSION: return "TYPED_EXPRESSION";
        case ANY: return "ANY";
    }
    return "UNKNOWN_EXPRESSION"; // Should not happen
}

void throwUnexpectedExpressionException(
    ExpressionType type, const char* location) {
    throw std::runtime_error(
        std::string{"Unexpected expression "} + getExpressionName(type) + " for " + location
    );
}

const Expression* EvaluatedDictionary::optionalLookup(size_t name) const {
    for (const auto& definition: definitions) {
        if (definition.name.global_index == name) {
            return &definition.expression;
        }
    }
    return nullptr;
}

Expression EvaluatedDictionary::lookup(size_t name) const {
    const auto expression = optionalLookup(name);
    if (expression == nullptr) {
        const auto name_c = storage.names.at(name).c_str();
        throwException("Cannot find name %s in dictionary", name_c);
    }
    return *expression;
}
