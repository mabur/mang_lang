#pragma once

#include "Expression.h"

DictionaryElements setContext(const DictionaryElements& elements);

void clearMemory();

NamedElement getNamedElement(ExpressionPointer expression);
WhileElement getWileElement(ExpressionPointer expression);
EndElement getEndElement(ExpressionPointer expression);
Number getNumber(ExpressionPointer expression);
Character getCharacter(ExpressionPointer expression);
Conditional getConditional(ExpressionPointer expression);
Dictionary getDictionary(ExpressionPointer expression);
Function getFunction(ExpressionPointer expression);
FunctionBuiltIn getFunctionBuiltIn(ExpressionPointer expression);
FunctionDictionary getFunctionDictionary(ExpressionPointer expression);
FunctionList getFunctionList(ExpressionPointer expression);
List getList(ExpressionPointer expression);
LookupChild getLokupChild(ExpressionPointer expression);
FunctionApplication getFunctionApplication(ExpressionPointer expression);
LookupSymbol getLookupSymbol(ExpressionPointer expression);
Name getName(ExpressionPointer expression);
String getString(ExpressionPointer expression);

ExpressionPointer makeNumber(const Number* expression);
ExpressionPointer makeCharacter(const Character* expression);
ExpressionPointer makeConditional(const Conditional* expression);
ExpressionPointer makeDictionary(const Dictionary* expression);
ExpressionPointer makeFunction(const Function* expression);
ExpressionPointer makeFunctionBuiltIn(const FunctionBuiltIn* expression);
ExpressionPointer makeFunctionDictionary(const FunctionDictionary* expression);
ExpressionPointer makeFunctionList(const FunctionList* expression);
ExpressionPointer makeList(const List* expression);
ExpressionPointer makeLookupChild(const LookupChild* expression);
ExpressionPointer makeFunctionApplication(const FunctionApplication* expression);
ExpressionPointer makeLookupSymbol(const LookupSymbol* expression);
ExpressionPointer makeName(const Name* expression);
ExpressionPointer makeString(const String* expression);
ExpressionPointer makeNamedElement(const NamedElement* expression);
ExpressionPointer makeWhileElement(const WhileElement* expression);
ExpressionPointer makeEndElement(const EndElement* expression);
