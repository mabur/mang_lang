#pragma once

#include <string>

#include "expression.h"

Statements setContext(const Statements& statements);

void clearMemory();

std::string getLog();

Definition getDefinition(Expression expression);
DynamicDefinition getDynamicDefinition(Expression expression);
WhileStatement getWileStatement(Expression expression);
EndStatement getEndStatement(Expression expression);
Number getNumber(Expression expression);
Character getCharacter(Expression expression);
Conditional getConditional(Expression expression);
IsExpression getIs(Expression expression);
Dictionary getDictionary(Expression expression);
EvaluatedDictionary getEvaluatedDictionary(Expression expression);
Function getFunction(Expression expression);
FunctionBuiltIn getFunctionBuiltIn(Expression expression);
FunctionDictionary getFunctionDictionary(Expression expression);
FunctionList getFunctionList(Expression expression);
List getList(Expression expression);
EvaluatedList getEvaluatedList(Expression expression);
EmptyList getEmptyList(Expression expression);
LookupChild getLookupChild(Expression expression);
FunctionApplication getFunctionApplication(Expression expression);
LookupSymbol getLookupSymbol(Expression expression);
DynamicLookupSymbol getDynamicLookupSymbol(Expression expression);
Name getName(Expression expression);
String getString(Expression expression);
EmptyString getEmptyString(Expression expression);
Boolean getBoolean(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeCharacter(CodeRange code, Character expression);
Expression makeConditional(CodeRange code, Conditional expression);
Expression makeIs(CodeRange code, IsExpression expression);
Expression makeDictionary(CodeRange code, const Dictionary expression);
Expression makeEvaluatedDictionary(CodeRange code, const EvaluatedDictionary* expression);
Expression makeFunction(CodeRange code, Function expression);
Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression);
Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression);
Expression makeFunctionList(CodeRange code, FunctionList expression);
Expression makeList(CodeRange code, List expression);
Expression makeEvaluatedList(CodeRange code, EvaluatedList expression);
Expression makeEmptyList(CodeRange code, EmptyList expression);
Expression makeLookupChild(CodeRange code, LookupChild expression);
Expression makeFunctionApplication(CodeRange code, FunctionApplication expression);
Expression makeLookupSymbol(CodeRange code, LookupSymbol expression);
Expression makeDynamicLookupSymbol(CodeRange code, DynamicLookupSymbol expression);
Expression makeName(CodeRange code, Name expression);
Expression makeDefinition(CodeRange code, Definition expression);
Expression makeDynamicDefinition(CodeRange code, DynamicDefinition expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeEndStatement(CodeRange code, EndStatement expression);
Expression makeString(CodeRange code, String expression);
Expression makeEmptyString(CodeRange code, EmptyString expression);
Expression makeBoolean(CodeRange code, Boolean expression);
