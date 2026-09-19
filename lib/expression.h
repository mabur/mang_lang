#pragma once

#include <map>
#include <stdint.h>
#include <string>

#include "expression_type.h"

typedef uint16_t CharacterIndex;

struct CodeRange {
    CharacterIndex data;
    CharacterIndex count;
};

// TODO: Pack tighter?
// Bit size           Current  Pack1  Pack2
// Expression::index       64     64   64-4
// Expression::range       32     32      0
// Expression::type        16      4      4
// Expression             128    128     64
struct Expression {
    size_t index = 0;
    CodeRange range = {};
    ExpressionType type = ANY_VALUE;
};

using Number = double;
using Character = char;
using ErrorValue = const char*;

struct Indices {
    size_t data;
    size_t count;
};

struct BoundGlobalName {
    size_t global_index; // Index to this name in the global storage.
    int parent_steps = -1; // Number of steps to parent. -1 if name resolution has not succeeded.
    size_t dictionary_index; // Index within the resolved dictionary.
};

// The name binds to a possibly mutable value in the currently constructed dictionary.
struct BoundLocalName {
    size_t global_index; // Index to this name in the global storage.
    size_t dictionary_index; // Index to this name and its data in the dictionary.
};

// TODO: type alias instead of struct?
struct DynamicExpression {
    Expression expression;
};

struct TypedExpression {
    Expression type;
    Expression value;
};

struct Alternative {
    Expression left;
    Expression right;
};

struct ConditionalExpression {
    Indices alternatives;
    Expression expression_else;
};

struct IsExpression {
    Expression input;
    Indices alternative;
    Expression expression_else;
};

// The three function expressions below store their arguments as two parallel
// ranges of the same count, filled in by the parser:
// - argument_names: a range into storage.slot_names, in slot order. It is
//   also the name range of the dictionary value a call builds as its frame.
// - argument_types: a range into storage.argument_types. An argument without
//   a type ascription has the any-value as its type.
struct FunctionExpression {
    Indices argument_names;
    Indices argument_types;
    Expression body;
};

typedef Expression (*FunctionPointer)(Expression);

struct FunctionBuiltInValue {
    FunctionPointer function;
};

struct FunctionDictionaryExpression {
    Indices argument_names;
    Indices argument_types;
    Expression body;
};

struct FunctionTupleExpression {
    Indices argument_names;
    Indices argument_types;
    Expression body;
};

struct FunctionValue {
    Expression function;
    Expression environment;
};

struct LookupChildExpression {
    size_t name;
    Expression child;
};

struct FunctionApplicationExpression {
    BoundGlobalName name;
    Expression child;
};

struct FunctionApplicationBuiltInExpression {
    size_t name;
    FunctionPointer function;
    FunctionPointer function_types;
    Expression child;
};

struct LookupSymbolExpression {
    BoundGlobalName name;
};

struct String {
    Expression top;
    Expression rest;
};

struct TupleExpression {
    Indices indices;
};

// TODO: add special case for tuple of size 2.
// TODO: merge with TupleExpression for storage but keep type-code to know if it is evaluated.
struct TupleValue {
    Indices indices;
};

struct StackExpression {
    Expression top;
    Expression rest;
};

// TODO: merge with StackExpression for storage but keep type-code to know if it is evaluated.
struct StackValue {
    Expression top;
    Expression rest;
};

// STATEMENTS BEGIN

struct DefinitionStatement {
    BoundLocalName name;
    Expression expression;
};

struct PutAssignmentStatement {
    BoundLocalName name;
    Expression expression;
};

struct PutEachAssignmentStatement {
    BoundLocalName name;
    Expression expression;
};

struct DropAssignmentStatement {
    BoundLocalName name;
};

struct WhileStatement {
    Expression expression;
    size_t end_index;
};

struct ForInitStatement {
    BoundLocalName name;
    Expression container_expression;
};

// The looping part of for:
struct ForStatement {
    BoundLocalName name;
    Expression container_expression;
    size_t end_index;
};

struct IfStatement {
    Expression expression;
    size_t end_index;
};

struct WhileEndStatement {
    size_t start_index;
};

struct ForEndStatement {
    size_t start_index;
};

// STATEMENTS END

struct ForIterator {
    Expression container;
};

struct DictionaryExpression {
    Indices statements;
    size_t definition_count;
    // The names defined by the statements, in slot order, as a range into
    // storage.slot_names. Shared by every value built from this
    // expression. Filled in by the resolver. names.count == definition_count.
    Indices names;
};

struct DictionaryValue {
    Expression environment;
    // The current values of the slots, as a range into storage.slot_values.
    Indices slot_values;
    // The names of the slots, in slot order, as a range into
    // storage.slot_names. Shared with the expression this value was
    // built from. names.count == slot_values.count.
    Indices names;
};

struct Row {
    Expression key;
    Expression value;
};

struct TableExpression {
    Indices rows;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct TableValue {
    using Iterator = std::map<std::string, Row>::const_iterator;
    std::map<std::string, Row> rows;
    Iterator begin() const {return rows.begin();}
    Iterator end() const {return rows.end();}
    bool empty() const {return rows.empty();}
};

struct TableViewValue {
    using Iterator = std::map<std::string, Row>::const_iterator;
    Iterator first;
    Iterator last;
    Iterator begin() const {return first;}
    Iterator end() const {return last;}
    bool empty() const {return first == last;}
};
