#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "parsing.h"
#include "SinglyLinkedList.h"

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    DICTIONARY,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_LIST,
    LIST,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    NAME,
    NAMED_ELEMENT,
    WHILE_ELEMENT,
    END_ELEMENT,
    NUMBER,
    STRING,
    EMPTY,
};

struct Character;
struct Conditional;
struct Dictionary;
struct NamedElement;
struct Expression;
struct Function;
struct FunctionBuiltIn;
struct FunctionDictionary;
struct FunctionList;
struct List;
struct LookupChild;
struct FunctionApplication;
struct LookupSymbol;
struct Name;
struct Number;
struct String;
struct WhileElement;
struct EndElement;

struct ExpressionPointer {
    ExpressionType type = EMPTY;
    size_t index = 0;
};

NamedElement namedElement(ExpressionPointer expression);
WhileElement whileElement(ExpressionPointer expression);
EndElement endElement(ExpressionPointer expression);
Number number2(ExpressionPointer expression);
Character character(ExpressionPointer expression);
Conditional conditional(ExpressionPointer expression);
Dictionary dictionary(ExpressionPointer expression);
Function function(ExpressionPointer expression);
FunctionBuiltIn functionBuiltIn(ExpressionPointer expression);
FunctionDictionary functionDictionary(ExpressionPointer expression);
FunctionList functionList(ExpressionPointer expression);
List list2(ExpressionPointer expression);
LookupChild lookupChild(ExpressionPointer expression);
FunctionApplication functionApplication(ExpressionPointer expression);
LookupSymbol lookupSymbol(ExpressionPointer expression);
Name name(ExpressionPointer expression);
String string(ExpressionPointer expression);

using InternalList = SinglyLinkedList<ExpressionPointer>;

struct Name;
using NamePointer = std::shared_ptr<const Name>;

struct Name {
    CodeRange range;
    std::string value;
};

struct Character {
    CodeRange range;
    char value;
};

struct Conditional {
    CodeRange range;
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
};

struct Function {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer input_name;
    ExpressionPointer body;
};

struct FunctionBuiltIn {
    CodeRange range;
    std::function<ExpressionPointer(ExpressionPointer)> function;
};

struct FunctionDictionary {
    CodeRange range;
    ExpressionPointer environment; // TODO: use this.
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};

struct FunctionList {
    CodeRange range;
    ExpressionPointer environment;
    std::vector<NamePointer> input_names;
    ExpressionPointer body;
};

struct List {
    CodeRange range;
    InternalList elements;
};

struct LookupChild {
    CodeRange range;
    NamePointer name;
    ExpressionPointer child;
};

struct FunctionApplication {
    CodeRange range;
    NamePointer name;
    ExpressionPointer child;
};

struct LookupSymbol {
    CodeRange range;
    NamePointer name;
};

struct Number {
    CodeRange range;
    double value;
};

struct String {
    CodeRange range;
    InternalList elements;
};

struct NamedElement {
    CodeRange range;
    NamePointer name;
    ExpressionPointer expression;
    size_t name_index_;
};

struct WhileElement {
    CodeRange range;
    ExpressionPointer expression;
    size_t end_index_;
};

struct EndElement {
    CodeRange range;
    size_t while_index_;
};

using DictionaryElements = std::vector<ExpressionPointer>;

struct Dictionary {
    CodeRange range;
    ExpressionPointer environment;
    DictionaryElements elements;
};

DictionaryElements setContext(const DictionaryElements& elements);

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
