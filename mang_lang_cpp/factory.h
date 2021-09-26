#pragma once

#include "Expression.h"

Statements setContext(const Statements& statements);

void clearMemory();

Definition getDefinition(Expression expression);
WhileStatement getWileStatement(Expression expression);
EndStatement getEndStatement(Expression expression);
Number getNumber(Expression expression);
Character getCharacter(Expression expression);
Conditional getConditional(Expression expression);
Dictionary getDictionary(Expression expression);
Function getFunction(Expression expression);
FunctionBuiltIn getFunctionBuiltIn(Expression expression);
FunctionDictionary getFunctionDictionary(Expression expression);
FunctionList getFunctionList(Expression expression);
NewList getNewList(Expression expression);
NewEmptyList getNewEmptyList(Expression expression);
LookupChild getLokupChild(Expression expression);
FunctionApplication getFunctionApplication(Expression expression);
LookupSymbol getLookupSymbol(Expression expression);
Name getName(Expression expression);
String getString(Expression expression);
EmptyString getEmptyString(Expression expression);

Expression makeNumber(const Number* expression);
Expression makeCharacter(const Character* expression);
Expression makeConditional(const Conditional* expression);
Expression makeDictionary(const Dictionary* expression);
Expression makeFunction(const Function* expression);
Expression makeFunctionBuiltIn(const FunctionBuiltIn* expression);
Expression makeFunctionDictionary(const FunctionDictionary* expression);
Expression makeFunctionList(const FunctionList* expression);
Expression makeNewList(const NewList* expression);
Expression makeNewEmptyList(const NewEmptyList* expression);
Expression makeLookupChild(const LookupChild* expression);
Expression makeFunctionApplication(const FunctionApplication* expression);
Expression makeLookupSymbol(const LookupSymbol* expression);
Expression makeName(const Name* expression);
Expression makeDefinition(const Definition* expression);
Expression makeWhileStatement(const WhileStatement* expression);
Expression makeEndStatement(const EndStatement* expression);
Expression makeString(const String* expression);
Expression makeEmptyString(const EmptyString* expression);
