#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "factory.h"
#include "parsing.h"
#include "SinglyLinkedList.h"

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
    ExpressionPointer environment;
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

struct LookupFunction {
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

struct DictionaryElement {
    CodeRange range;
    ExpressionPointer environment;
    NamePointer name;
    ExpressionPointer expression;
    size_t while_index_;
    size_t end_index_;
    size_t name_index_;
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
