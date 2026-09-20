#pragma once

#include <stdarg.h>
#include <vector>

#include <carma/carma_string.h>

#include "expression.h"

#define DARRAY(type) struct {type* data; size_t count; size_t capacity;}

struct Expressions {
    Expression* data;
    size_t count;
    size_t capacity;
};

struct NameIndex {
    StringView key;
    size_t value;
};

struct Storage {
    // SOA for CodeCharacter:
    DARRAY(char) code_characters;
    DARRAY(CharacterIndex) code_rows;
    DARRAY(CharacterIndex) code_columns;

    DARRAY(DynamicExpression) dynamic_expressions;
    DARRAY(TypedExpression) typed_expressions;
    DARRAY(DictionaryExpression) dictionary_expressions;
    // Dictionary values that are never reclaimed:
    DARRAY(DictionaryValue) dictionary_values_forever;
    // Dictionary values that are reclaimed when the scope that built them
    // ends. Pushed and popped in stack order, like their slot_values_stack.
    DARRAY(DictionaryValue) dictionary_values_stack;
    DARRAY(ConditionalExpression) conditional_expressions;
    DARRAY(IsExpression) is_expressions;
    DARRAY(Alternative) alternatives;
    DARRAY(FunctionExpression) function_expressions;
    DARRAY(FunctionBuiltInValue) function_built_in_values;
    DARRAY(FunctionDictionaryExpression) function_dictionary_expressions;
    DARRAY(FunctionTupleExpression) function_tuple_expressions;
    DARRAY(FunctionValue) function_values;
    DARRAY(TupleExpression) tuple_expressions;
    DARRAY(TupleValue) tuple_values;
    DARRAY(StackExpression) stack_expressions;
    DARRAY(StackValue) stack_values;
    DARRAY(TableViewValue) table_view_values;
    DARRAY(LookupChildExpression) lookup_child_expressions;
    DARRAY(FunctionApplicationExpression) function_application_expressions;
    DARRAY(FunctionApplicationBuiltInExpression) function_application_built_in_expressions;
    DARRAY(LookupSymbolExpression) lookup_symbol_expressions;
    // The type ascriptions of all function arguments. An argument without an
    // ascription has the any-value here. Parallel to its name in slot_names.
    DARRAY(Expression) argument_types;
    DARRAY(WhileStatement) while_statements;
    DARRAY(ForInitStatement) for_init_statements;
    DARRAY(ForStatement) for_statements;
    DARRAY(IfStatement) if_statements;
    DARRAY(WhileEndStatement) while_end_statements;
    DARRAY(ForEndStatement) for_end_statements;
    DARRAY(DefinitionStatement) definition_statements;
    DARRAY(PutAssignmentStatement) put_assignment_statements;
    DARRAY(PutEachAssignmentStatement) put_each_assignment_statements;
    DARRAY(DropAssignmentStatement) drop_assignment_statements;
    DARRAY(Expression) statements;
    DARRAY(Expression) expressions;
    // The slot names of all dictionary_expressions. Shared with dictionary values.
    // Also used for function arguments.
    DARRAY(size_t) slot_names;
    // The slot values of dictionary_values_forever and dictionary_values_stack:
    DARRAY(Expression) slot_values_forever;
    DARRAY(Expression) slot_values_stack;
    DARRAY(String) strings;
    DARRAY(Row) rows;
    DARRAY(TableExpression) table_expressions;
    
    // Null-terminated strings concatenated after each other:
    StringBuilder names;

    DARRAY(NameIndex) name_index_table;
    
    std::vector<TableValue> table_values;
};

extern Storage storage;

void clearMemory();

// Prints the count and size of every non-empty storage array to stdout.
// Useful to see what a program allocates.
void printStorageStatistics();

Character getCharacter(Expression expression);
Number getNumber(Expression expression);
ErrorValue getErrorValue(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeErrorValue(CodeRange code, const char* format, ...);
Expression makeCharacter(CodeRange code, Character expression);
Expression makeDynamicExpression(CodeRange code, DynamicExpression expression);
Expression makeTypedExpression(CodeRange code, TypedExpression expression);
Expression makeConditionalExpression(CodeRange code, ConditionalExpression expression);
Expression makeIsExpression(CodeRange code, IsExpression expression);
Expression makeAlternative(CodeRange code, Alternative expression);
Expression makeDictionaryExpression(CodeRange code, DictionaryExpression expression);
Expression makeDictionaryValueForever(CodeRange code, DictionaryValue expression);
Expression makeDictionaryValueStack(CodeRange code, DictionaryValue expression);
Expression makeFunctionExpression(CodeRange code, FunctionExpression expression);
Expression makeFunctionBuiltInValue(CodeRange code, FunctionBuiltInValue expression);
Expression makeFunctionDictionaryExpression(CodeRange code, FunctionDictionaryExpression expression);
Expression makeFunctionTupleExpression(CodeRange code, FunctionTupleExpression expression);
Expression makeFunctionValue(CodeRange code, FunctionValue expression);
Expression makeTupleExpression(CodeRange code, TupleExpression expression);
Expression makeTupleValue(CodeRange code, TupleValue expression);
Expression makeTupleValue2(Expression a, Expression b);
Expression makeStackExpression(CodeRange code, StackExpression expression);
Expression makeStackValue(CodeRange code, StackValue expression);
Expression makeTableExpression(CodeRange code, TableExpression expression);
Expression makeTableValue(CodeRange code, TableValue expression);
Expression makeTableViewValue(CodeRange code, TableViewValue expression);
Expression makeLookupChildExpression(CodeRange code, LookupChildExpression expression);
Expression makeFunctionApplicationExpression(CodeRange code, FunctionApplicationExpression expression);
Expression makeFunctionApplicationBuiltInExpression(CodeRange code, FunctionApplicationBuiltInExpression expression);
Expression makeLookupSymbolExpression(CodeRange code, LookupSymbolExpression expression);
Expression makeName(CodeRange code, const char* data, size_t count);
Expression makeDefinitionStatement(CodeRange code, DefinitionStatement expression);
Expression makePutAssignmentStatement(CodeRange code, PutAssignmentStatement expression);
Expression makePutEachAssignmentStatement(CodeRange code, PutEachAssignmentStatement expression);
Expression makeDropAssignmentStatement(CodeRange code, DropAssignmentStatement expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeForInitStatement(CodeRange code, ForInitStatement expression);
Expression makeForStatement(CodeRange code, ForStatement expression);
Expression makeIfStatement(CodeRange code, IfStatement expression);
Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression);
Expression makeForEndStatement(CodeRange code, ForEndStatement expression);
Expression makeString(CodeRange code, String expression);

CodeRange makeCodeCharacters(const char* s);

char firstCharacter(CodeRange code);
size_t firstColumn(CodeRange code);
size_t firstRow(CodeRange code);
char lastCharacter(CodeRange code);
size_t lastColumn(CodeRange code);
size_t lastRow(CodeRange code);
