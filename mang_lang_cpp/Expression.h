#pragma once

#include <memory>
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
    NEW_STRING,
    NEW_EMPTY_STRING,
    EMPTY,
};

struct ExpressionPointer {
    ExpressionType type = EMPTY;
    size_t index = 0;
};

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

struct NewString {
    CodeRange range;
    ExpressionPointer first;
    ExpressionPointer rest;
};

struct NewEmptyString {
    CodeRange range;
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
