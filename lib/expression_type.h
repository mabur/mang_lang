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
    DICTIONARY_VALUE,
    TABLE_VALUE,
    TABLE_VIEW_VALUE,
    FUNCTION_VALUE,
    FUNCTION_BUILT_IN_VALUE,
    ANY_VALUE,
    ERROR_VALUE,

    // INTERNAL RUNTIME STATE
    // Stored in dictionary slots while a for loop runs. Never returned by
    // evaluation, since lookups convert it with take.
    FOR_ITERATOR,

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
    CONDITIONAL,
    IS,
    LOOKUP_SYMBOL,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    FUNCTION_APPLICATION_BUILT_IN,
    TYPED_EXPRESSION,
    DYNAMIC_EXPRESSION,

    // Fragments that are never evaluated on their own:
    NAME,
    ARGUMENT,
    ALTERNATIVE,

    // Statements that are executed while constructing a dictionary value:
    DEFINITION,
    PUT_ASSIGNMENT,
    PUT_EACH_ASSIGNMENT,
    DROP_ASSIGNMENT,
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

const char* getExpressionName(ExpressionType type);

// The name printed for a function type, given the tag of the function
// expression a function value was created from. Falls back to
// getExpressionName for anything else.
const char* getFunctionTypeName(ExpressionType type);
