#pragma once

#include <map>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "expression_type.h"

typedef uint16_t CharacterIndex;

struct CodeCharacter {
    char character = 'a';
    CharacterIndex row = 0;
    CharacterIndex column = 0;
};

struct CodeRange {
    CharacterIndex data;
    CharacterIndex count;
};

// TODO: Pack tighter?
// Bit size           Current  Pack1  Pack2
// Expression::type        16      4      4
// Expression::index       64     64   64-4
// Expression::range       32     32      0
// Expression             128    128     64
struct Expression {
    ExpressionType type = ANY;
    size_t index = 0;
    CodeRange range;
};

using Number = double;
using Character = char;
using Name = std::string;

struct BoundGlobalName {
    size_t global_index; // Index to this name in the global storage.
    int parent_steps = -1; // Number of steps to parent. -1 if unresolved yet.
};

struct BoundLocalName {
    size_t global_index; // Index to this name in the global storage.
    size_t dictionary_index; // Index to this name and its data in the dictionary.
};

struct Argument {
    Expression type; // Optional
    size_t name;
};

// TODO: type alias instead of struct?
struct DynamicExpression {
    Expression expression;
};

struct TypedExpression {
    BoundGlobalName type_name;
    Expression value;
};

struct Alternative {
    Expression left;
    Expression right;
};

struct Conditional {
    Expression alternative_first;
    Expression alternative_last;
    Expression expression_else;
};

struct IsExpression {
    Expression input;
    Expression alternative_first;
    Expression alternative_last;
    Expression expression_else;
};

struct Function {
    Expression environment;
    size_t argument;
    Expression body;
};

typedef Expression (*FunctionPointer)(Expression);

struct FunctionBuiltIn {
    FunctionPointer function;
};

struct FunctionDictionary {
    Expression environment; // TODO: use this.
    size_t first_argument;
    size_t last_argument; // Exclusive, to handle empty range
    Expression body;
};

struct FunctionTuple {
    Expression environment;
    size_t first_argument;
    size_t last_argument; // Exclusive, to handle empty range
    Expression body;
};

struct LookupChild {
    size_t name;
    Expression child;
};

struct FunctionApplication {
    BoundGlobalName name;
    Expression child;
};

struct LookupSymbol {
    BoundGlobalName name;
};

struct String {
    Expression top;
    Expression rest;
};

struct Tuple {
    size_t first;
    size_t last;
};

// TODO: add special case for tuple of size 2.
// TODO: merge with Tuple for storage but keep type-code to know if it is evaluated.
struct EvaluatedTuple {
    size_t first;
    size_t last;
};

struct Stack {
    Expression top;
    Expression rest;
};

// TODO: merge with Stack for storage but keep type-code to know if it is evaluated.
struct EvaluatedStack {
    Expression top;
    Expression rest;
};

// STATEMENTS BEGIN

struct Definition {
    BoundLocalName name;
    Expression expression;
};

struct PutAssignment {
    BoundLocalName name;
    Expression expression;
};

struct PutEachAssignment {
    BoundLocalName name;
    Expression expression;
};

struct DropAssignment {
    BoundLocalName name;
};

struct WhileStatement {
    Expression expression;
    size_t end_index;
};

struct ForStatement {
    BoundLocalName item_name;
    BoundLocalName container_name;
    size_t end_index;
};

struct ForSimpleStatement {
    BoundLocalName container_name;
    size_t end_index;
};

struct WhileEndStatement {
    size_t start_index;
};

struct ForEndStatement {
    size_t start_index;
};

struct ForSimpleEndStatement {
    size_t start_index;
};

// STATEMENTS END

struct Dictionary {
    size_t statement_first;
    size_t statement_last;
    size_t definition_count;
};

// TODO: make cheaper to copy.
// Bottleneck.
// Use index range for definitions.
// But then need to know all members of the dictionary before it is evaluated.
// so that they are all added together, at least the first time.
// Alternatively, use unordered_map for fast lookup of all names
// and not just definitions.
// Bottleneck.
/*
struct EvaluatedDictionary {
    Expression environment;
    size_t name_first; // Index to constant names/keys. Could possibly be shared across instances.
    size_t name_last;  // Index to constant names/keys. Could possibly be shared across instances.
    size_t value_first; // Index to values/expression that can change during the evaluation.
    size_t value_last;  // Index to values/expression that can change during the evaluation.
};
 */
struct EvaluatedDictionary {
    EvaluatedDictionary(const EvaluatedDictionary&) = delete;
    EvaluatedDictionary(EvaluatedDictionary&&) = default;
    EvaluatedDictionary& operator=(const EvaluatedDictionary&) = delete;
    EvaluatedDictionary& operator=(EvaluatedDictionary&&) = default;

    Expression environment;
    std::vector<Definition> definitions;

    const Expression* optionalLookup(size_t name) const;
    Expression lookup(size_t name) const;
};

struct Row {
    Expression key;
    Expression value;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct Table {
    std::vector<Row> rows;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct EvaluatedTable {
    using Iterator = std::map<std::string, Row>::const_iterator;
    std::map<std::string, Row> rows;
    Iterator begin() const {return rows.begin();}
    Iterator end() const {return rows.end();}
    bool empty() const {return rows.empty();}
};

struct EvaluatedTableView {
    using Iterator = std::map<std::string, Row>::const_iterator;
    Iterator first;
    Iterator last;
    Iterator begin() const {return first;}
    Iterator end() const {return last;}
    bool empty() const {return first == last;}
};
