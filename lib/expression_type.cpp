#include "expression_type.h"

const char* getExpressionName(ExpressionType type) {
    switch (type) {
        // Values, self-evaluating:
        case NUMBER: return "NUMBER";
        case CHARACTER: return "CHARACTER";
        case YES: return "YES";
        case NO: return "NO";
        case STRING: return "STRING";
        case EMPTY_STRING: return "EMPTY_STRING";
        case EMPTY_STACK: return "EMPTY_STACK";

        // Values, produced by evaluation:
        case STACK_VALUE: return "STACK_VALUE";
        case TUPLE_VALUE: return "TUPLE_VALUE";
        case DICTIONARY_VALUE: return "DICTIONARY_VALUE";
        case TABLE_VALUE: return "TABLE_VALUE";
        case TABLE_VIEW_VALUE: return "TABLE_VIEW_VALUE";
        case FUNCTION_VALUE: return "FUNCTION_VALUE";
        case FUNCTION_BUILT_IN_VALUE: return "FUNCTION_BUILT_IN_VALUE";
        case ANY_VALUE: return "ANY_VALUE";
        case ERROR_VALUE: return "ERROR_VALUE";

        // Internal runtime state:
        case FOR_ITERATOR: return "FOR_ITERATOR";

        // Expressions, constructors:
        case STACK_EXPRESSION: return "STACK_EXPRESSION";
        case TUPLE_EXPRESSION: return "TUPLE_EXPRESSION";
        case DICTIONARY_EXPRESSION: return "DICTIONARY_EXPRESSION";
        case TABLE_EXPRESSION: return "TABLE_EXPRESSION";
        case FUNCTION_EXPRESSION: return "FUNCTION_EXPRESSION";
        case FUNCTION_TUPLE_EXPRESSION: return "FUNCTION_TUPLE_EXPRESSION";
        case FUNCTION_DICTIONARY_EXPRESSION: return "FUNCTION_DICTIONARY_EXPRESSION";

        // Expressions, operations:
        case CONDITIONAL_EXPRESSION: return "CONDITIONAL_EXPRESSION";
        case IS_EXPRESSION: return "IS_EXPRESSION";
        case LOOKUP_SYMBOL_EXPRESSION: return "LOOKUP_SYMBOL_EXPRESSION";
        case LOOKUP_CHILD_EXPRESSION: return "LOOKUP_CHILD_EXPRESSION";
        case FUNCTION_APPLICATION_EXPRESSION: return "FUNCTION_APPLICATION_EXPRESSION";
        case FUNCTION_APPLICATION_BUILT_IN_EXPRESSION: return "FUNCTION_APPLICATION_BUILT_IN_EXPRESSION";
        case TYPED_EXPRESSION: return "TYPED_EXPRESSION";
        case DYNAMIC_EXPRESSION: return "DYNAMIC_EXPRESSION";

        // Syntax fragments:
        case NAME: return "NAME";
        case ALTERNATIVE: return "ALTERNATIVE";

        // Statements:
        case DEFINITION_STATEMENT: return "DEFINITION_STATEMENT";
        case PUT_ASSIGNMENT_STATEMENT: return "PUT_ASSIGNMENT_STATEMENT";
        case PUT_EACH_ASSIGNMENT_STATEMENT: return "PUT_EACH_ASSIGNMENT_STATEMENT";
        case DROP_ASSIGNMENT_STATEMENT: return "DROP_ASSIGNMENT_STATEMENT";
        case WHILE_STATEMENT: return "WHILE_STATEMENT";
        case FOR_INIT_STATEMENT: return "FOR_INIT_STATEMENT";
        case FOR_STATEMENT: return "FOR_STATEMENT";
        case IF_STATEMENT: return "IF_STATEMENT";
        case WHILE_END_STATEMENT: return "WHILE_END_STATEMENT";
        case FOR_END_STATEMENT: return "FOR_END_STATEMENT";
        case IF_END_STATEMENT: return "IF_END_STATEMENT";
        case END_STATEMENT: return "END_STATEMENT";
        case RETURN_STATEMENT: return "RETURN_STATEMENT";
    }
    return "UNKNOWN_EXPRESSION"; // Should not happen
}

const char* getFunctionTypeName(ExpressionType type) {
    switch (type) {
        case FUNCTION_EXPRESSION: return "FUNCTION";
        case FUNCTION_TUPLE_EXPRESSION: return "FUNCTION_TUPLE";
        case FUNCTION_DICTIONARY_EXPRESSION: return "FUNCTION_DICTIONARY";
        default: return getExpressionName(type);
    }
}
