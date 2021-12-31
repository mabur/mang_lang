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
Function getFunction(Expression expression);
FunctionBuiltIn getFunctionBuiltIn(Expression expression);
FunctionDictionary getFunctionDictionary(Expression expression);
FunctionList getFunctionList(Expression expression);
List getList(Expression expression);
EmptyList getEmptyList(Expression expression);
LookupChild getLookupChild(Expression expression);
FunctionApplication getFunctionApplication(Expression expression);
LookupSymbol getLookupSymbol(Expression expression);
Name getName(Expression expression);
String getString(Expression expression);
EmptyString getEmptyString(Expression expression);
Boolean getBoolean(Expression expression);

Expression makeNumber(Number expression);
Expression makeCharacter(Character expression);
Expression makeConditional(Conditional expression);
Expression makeIs(IsExpression expression);
Expression makeDictionary(const Dictionary* expression);
Expression makeFunction(Function expression);
Expression makeFunctionBuiltIn(FunctionBuiltIn expression);
Expression makeFunctionDictionary(FunctionDictionary expression);
Expression makeFunctionList(FunctionList expression);
Expression makeList(List expression);
Expression makeEmptyList(EmptyList expression);
Expression makeLookupChild(LookupChild expression);
Expression makeFunctionApplication(FunctionApplication expression);
Expression makeLookupSymbol(LookupSymbol expression);
Expression makeName(Name expression);
Expression makeDefinition(Definition expression);
Expression makeWhileStatement(WhileStatement expression);
Expression makeEndStatement(EndStatement expression);
Expression makeString(String expression);
Expression makeEmptyString(EmptyString expression);
Expression makeBoolean(Boolean expression);
