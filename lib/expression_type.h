#pragma once

// The tags are grouped so that every tag evaluation may return, a value,
// forms one contiguous block at the start of the enum. Keep new tags inside
// the section they belong to.
enum ExpressionType {
    // Values that evaluate to themselves:
    NUMBER,
    CHARACTER,
    YES,
    NO,
    STRING,
    EMPTY_STRING,
    EMPTY_STACK,

    // Values that are produced by evaluating an expression:
    STACK_VALUE,
    TUPLE_VALUE,
    // A dictionary value has one of two lifetimes, and lives in the storage
    // arrays named after it. FOREVER: never reclaimed. STACK: reclaimed when
    // the scope that built it ends, which is what an argument frame gets.
    DICTIONARY_VALUE_FOREVER,
    DICTIONARY_VALUE_STACK,
    TABLE_VALUE,
    TABLE_VIEW_VALUE,
    FUNCTION_VALUE,
    FUNCTION_BUILT_IN_VALUE,
    ANY_VALUE,
    ERROR_VALUE,

    // EXPRESSIONS:

    // Constructors. Each has a value twin above:
    STACK_EXPRESSION,
    TUPLE_EXPRESSION,
    DICTIONARY_EXPRESSION,
    TABLE_EXPRESSION,
    FUNCTION_EXPRESSION,
    FUNCTION_TUPLE_EXPRESSION,
    FUNCTION_DICTIONARY_EXPRESSION,

    // Operations:
    CONDITIONAL_EXPRESSION,
    IS_EXPRESSION,
    LOOKUP_SYMBOL_EXPRESSION,
    LOOKUP_CHILD_EXPRESSION,
    FUNCTION_APPLICATION_EXPRESSION,
    FUNCTION_APPLICATION_BUILT_IN_EXPRESSION,
    TYPED_EXPRESSION,
    DYNAMIC_EXPRESSION,

    // Fragments that are never evaluated on their own:
    NAME,
    ALTERNATIVE,
    
    DEFINITION_STATEMENT, // Used for both DictionaryExpression and DictionaryValue.

    // These statements are used inside DictionaryExpressions:
    PUT_ASSIGNMENT_STATEMENT,
    PUT_EACH_ASSIGNMENT_STATEMENT,
    DROP_ASSIGNMENT_STATEMENT,
    WHILE_STATEMENT,
    FOR_INIT_STATEMENT,
    FOR_STATEMENT,
    IF_STATEMENT,
    WHILE_END_STATEMENT,
    FOR_END_STATEMENT,
    IF_END_STATEMENT,
    END_STATEMENT,
    RETURN_STATEMENT,
};

inline bool isDictionaryValue(ExpressionType type) {
    return type == DICTIONARY_VALUE_FOREVER || type == DICTIONARY_VALUE_STACK;
}

const char* getExpressionName(ExpressionType type);

// The name printed for a function type, given the tag of the function
// expression a function value was created from. Falls back to
// getExpressionName for anything else.
const char* getFunctionTypeName(ExpressionType type);
