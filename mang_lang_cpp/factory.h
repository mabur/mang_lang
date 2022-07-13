#pragma once

#include <string>

#include "expression.h"

Statements setContext(const Statements& statements);
void clearMemory();
std::string getLog();

Definition getDefinition(Expression expression);
WhileStatement getWileStatement(Expression expression);
EndStatement getEndStatement(Expression expression);
Number getNumber(Expression expression);
Character getCharacter(Expression expression);
Conditional getConditional(Expression expression);
IsExpression getIs(Expression expression);
Dictionary getDictionary(Expression expression);
EvaluatedDictionary getEvaluatedDictionary(Expression expression);
EvaluatedDictionary& getMutableEvaluatedDictionary(Expression expression);
Table getTable(Expression expression);
EvaluatedTable getEvaluatedTable(Expression expression);
Tuple getTuple(Expression expression);
EvaluatedTuple getEvaluatedTuple(Expression expression);
Function getFunction(Expression expression);
FunctionBuiltIn getFunctionBuiltIn(Expression expression);
FunctionDictionary getFunctionDictionary(Expression expression);
FunctionTuple getFunctionTuple(Expression expression);
Stack getStack(Expression expression);
EvaluatedStack getEvaluatedStack(Expression expression);
EmptyStack getEmptyStack(Expression expression);
LookupChild getLookupChild(Expression expression);
FunctionApplication getFunctionApplication(Expression expression);
LookupSymbol getLookupSymbol(Expression expression);
Name getName(Expression expression);
Label getLabel(Expression expression);
String getString(Expression expression);
EmptyString getEmptyString(Expression expression);
Boolean getBoolean(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeCharacter(CodeRange code, Character expression);
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
Expression makeEmptyStack(CodeRange code, EmptyStack expression);
Expression makeTable(CodeRange code, Table expression);
Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression);
Expression makeLookupChild(CodeRange code, LookupChild expression);
Expression makeFunctionApplication(CodeRange code, FunctionApplication expression);
Expression makeLookupSymbol(CodeRange code, LookupSymbol expression);
Expression makeName(CodeRange code, Name expression);
Expression makeLabel(CodeRange code, Label expression);
Expression makeDefinition(CodeRange code, Definition expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeEndStatement(CodeRange code, EndStatement expression);
Expression makeString(CodeRange code, String expression);
Expression makeEmptyString(CodeRange code, EmptyString expression);
Expression makeBoolean(CodeRange code, Boolean expression);
