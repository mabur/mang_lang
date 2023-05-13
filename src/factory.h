#pragma once

#include <string>

#include "expression.h"

struct BinaryTuple {
    Expression left;
    Expression right;
};

BinaryTuple getBinaryTuple(Expression in);
void clearMemory();
std::string getLog();

DynamicExpression getDynamicExpression(Expression expression);
TypedExpression getTypedExpression(Expression expression);
Definition getDefinition(Expression expression);
PutAssignment getPutAssignment(Expression expression);
DropAssignment getDropAssignment(Expression expression);
WhileStatement getWhileStatement(Expression expression);
WhileStatement& getMutableWhileStatement(Expression expression);
ForStatement getForStatement(Expression expression);
ForSimpleStatement getForSimpleStatement(Expression expression);
ForStatement& getMutableForStatement(Expression expression);
ForSimpleStatement& getMutableForSimpleStatement(Expression expression);
WhileEndStatement getWhileEndStatement(Expression expression);
ForEndStatement getForEndStatement(Expression expression);
Number getNumber(Expression expression);
Character getCharacter(Expression expression);
Conditional getConditional(Expression expression);
IsExpression getIs(Expression expression);
Dictionary getDictionary(Expression expression);
EvaluatedDictionary getEvaluatedDictionary(Expression expression);
EvaluatedDictionary& getMutableEvaluatedDictionary(Expression expression);
Table getTable(Expression expression);
const EvaluatedTable& getEvaluatedTable(Expression expression);
EvaluatedTable& getMutableEvaluatedTable(Expression expression);
EvaluatedTableView getEvaluatedTableView(Expression expression);
Tuple getTuple(Expression expression);
EvaluatedTuple getEvaluatedTuple(Expression expression);
Function getFunction(Expression expression);
FunctionBuiltIn getFunctionBuiltIn(Expression expression);
FunctionDictionary getFunctionDictionary(Expression expression);
FunctionTuple getFunctionTuple(Expression expression);
Stack getStack(Expression expression);
EvaluatedStack getEvaluatedStack(Expression expression);
LookupChild getLookupChild(Expression expression);
FunctionApplication getFunctionApplication(Expression expression);
LookupSymbol getLookupSymbol(Expression expression);
NamePointer getName(Expression expression);
Argument getArgument(Expression expression);
String getString(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeCharacter(CodeRange code, Character expression);
Expression makeDynamicExpression(CodeRange code, DynamicExpression expression);
Expression makeTypedExpression(CodeRange code, TypedExpression expression);
Expression makeConditional(CodeRange code, Conditional expression);
Expression makeIs(CodeRange code, IsExpression expression);
Expression makeDictionary(CodeRange code, Dictionary expression);
Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression);
Expression makeFunction(CodeRange code, Function expression);
Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression);
Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression);
Expression makeFunctionTuple(CodeRange code, FunctionTuple expression);
Expression makeTuple(CodeRange code, Tuple expression);
Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression);
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
Expression makeDropAssignment(CodeRange code, DropAssignment expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeForStatement(CodeRange code, ForStatement expression);
Expression makeForSimpleStatement(CodeRange code, ForSimpleStatement expression);
Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression);
Expression makeForEndStatement(CodeRange code, ForEndStatement expression);
Expression makeString(CodeRange code, String expression);
