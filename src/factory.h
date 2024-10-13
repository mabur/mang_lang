#pragma once

#include "expression.h"

#define DARRAY(type) struct {type* data; size_t count; size_t capacity;}

struct Expressions {
    Expression* data;
    size_t count;
    size_t capacity;
};

struct Storage {
    // TODO: store the source code and CodeRanges here as well.

    DARRAY(DynamicExpression) dynamic_expressions;
    DARRAY(TypedExpression) typed_expressions;
    DARRAY(Dictionary) dictionaries;
    DARRAY(Conditional) conditionals;
    DARRAY(IsExpression) is_expressions;
    DARRAY(Alternative) alternatives;
    DARRAY(Function) functions;
    DARRAY(FunctionBuiltIn) built_in_functions;
    DARRAY(FunctionDictionary) dictionary_functions;
    DARRAY(FunctionTuple) tuple_functions;
    DARRAY(Tuple) tuples;
    DARRAY(EvaluatedTuple) evaluated_tuples;
    DARRAY(Stack) stacks;
    DARRAY(EvaluatedStack) evaluated_stacks;
    DARRAY(EvaluatedTableView) evaluated_table_views;
    DARRAY(LookupChild) child_lookups;
    DARRAY(FunctionApplication) function_applications;
    DARRAY(LookupSymbol) symbol_lookups;
    DARRAY(Argument) arguments;
    DARRAY(WhileStatement) while_statements;
    DARRAY(ForStatement) for_statements;
    DARRAY(ForSimpleStatement) for_simple_statements;
    DARRAY(WhileEndStatement) while_end_statements;
    DARRAY(ForEndStatement) for_end_statements;
    DARRAY(ForSimpleEndStatement) for_simple_end_statements;
    DARRAY(Definition) definitions;
    DARRAY(PutAssignment) put_assignments;
    DARRAY(PutEachAssignment) put_each_assignments;
    DARRAY(DropAssignment) drop_assignments;
    DARRAY(Expression) statements;
    DARRAY(Expression) expressions;
    DARRAY(String) strings;

    std::vector<Name> names;
    std::unordered_map<Name, size_t> name_indices;
    std::vector<EvaluatedDictionary> evaluated_dictionaries;
    std::vector<Table> tables;
    std::vector<EvaluatedTable> evaluated_tables;
};

extern Storage storage;

void clearMemory();

Character getCharacter(Expression expression);
Number getNumber(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeCharacter(CodeRange code, Character expression);
Expression makeDynamicExpression(CodeRange code, DynamicExpression expression);
Expression makeTypedExpression(CodeRange code, TypedExpression expression);
Expression makeConditional(CodeRange code, Conditional expression);
Expression makeIs(CodeRange code, IsExpression expression);
Expression makeAlternative(CodeRange code, Alternative expression);
Expression makeDictionary(CodeRange code, Dictionary expression);
Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression);
Expression makeFunction(CodeRange code, Function expression);
Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression);
Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression);
Expression makeFunctionTuple(CodeRange code, FunctionTuple expression);
Expression makeTuple(CodeRange code, Tuple expression);
Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression);
Expression makeEvaluatedTuple2(Expression a, Expression b);
Expression makeStack(CodeRange code, Stack expression);
Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression);
Expression makeTable(CodeRange code, Table expression);
Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression);
Expression makeEvaluatedTableView(CodeRange code, EvaluatedTableView expression);
Expression makeLookupChild(CodeRange code, LookupChild expression);
Expression makeFunctionApplication(CodeRange code, FunctionApplication expression);
Expression makeLookupSymbol(CodeRange code, LookupSymbol expression);
Expression makeName(CodeRange code, Name expression);
Expression makeArgument(CodeRange code, Argument expression);
Expression makeDefinition(CodeRange code, Definition expression);
Expression makePutAssignment(CodeRange code, PutAssignment expression);
Expression makePutEachAssignment(CodeRange code, PutEachAssignment expression);
Expression makeDropAssignment(CodeRange code, DropAssignment expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeForStatement(CodeRange code, ForStatement expression);
Expression makeForSimpleStatement(CodeRange code, ForSimpleStatement expression);
Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression);
Expression makeForEndStatement(CodeRange code, ForEndStatement expression);
Expression makeForSimpleEndStatement(CodeRange code, ForSimpleEndStatement expression);
Expression makeString(CodeRange code, String expression);

CodeRange makeCodeCharacters(const char* s);
