#pragma once

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "parsing.h"

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    IS,
    ALTERNATIVE,
    TABLE,
    EVALUATED_TABLE,
    EVALUATED_TABLE_VIEW,
    DICTIONARY,
    EVALUATED_DICTIONARY,
    TUPLE,
    EVALUATED_TUPLE,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_TUPLE,
    STACK,
    EVALUATED_STACK,
    EMPTY_STACK,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    NAME,
    ARGUMENT,
    DEFINITION,
    PUT_ASSIGNMENT,
    PUT_EACH_ASSIGNMENT,
    DROP_ASSIGNMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    FOR_SIMPLE_STATEMENT,
    WHILE_END_STATEMENT,
    FOR_END_STATEMENT,
    RETURN_STATEMENT,
    NUMBER,
    STRING,
    EMPTY_STRING,
    YES,
    NO,
    DYNAMIC_EXPRESSION,
    TYPED_EXPRESSION,
    ANY,
};

const auto NAMES = std::vector<std::string>{
    "CHARACTER",
    "CONDITIONAL",
    "IS",
    "ALTERNATIVE",
    "TABLE",
    "EVALUATED_TABLE",
    "EVALUATED_TABLE_VIEW",
    "DICTIONARY",
    "EVALUATED_DICTIONARY",
    "TUPLE",
    "EVALUATED_TUPLE",
    "FUNCTION",
    "FUNCTION_BUILT_IN",
    "FUNCTION_DICTIONARY",
    "FUNCTION_TUPLE",
    "STACK",
    "EVALUATED_STACK",
    "EMPTY_STACK",
    "LOOKUP_CHILD",
    "FUNCTION_APPLICATION",
    "LOOKUP_SYMBOL",
    "NAME",
    "ARGUMENT",
    "DEFINITION",
    "PUT_ASSIGNMENT",
    "PUT_EACH_ASSIGNMENT",
    "DROP_ASSIGNMENT",
    "WHILE_STATEMENT",
    "FOR_STATEMENT",
    "FOR_SIMPLE_STATEMENT,",
    "WHILE_END_STATEMENT",
    "FOR_END_STATEMENT",
    "RETURN_STATEMENT",
    "NUMBER",
    "STRING",
    "EMPTY_STRING",
    "YES",
    "NO",
    "DYNAMIC_EXPRESSION",
    "TYPED_EXPRESSION",
    "ANY",
};

struct StaticTypeError : public std::runtime_error
{
    StaticTypeError(ExpressionType type, const std::string& location);
    using runtime_error::runtime_error;
};

struct UnexpectedExpression : public std::runtime_error
{
    UnexpectedExpression(ExpressionType type, const std::string& location);
    using runtime_error::runtime_error;
};

struct MissingSymbol : public std::runtime_error
{
    MissingSymbol(const std::string& symbol, const std::string& location);
    using runtime_error::runtime_error;
};

struct MissingKey : public std::runtime_error
{
    MissingKey(const std::string& key);
    using runtime_error::runtime_error;
};

struct Expression {
    ExpressionType type = ANY;
    size_t index = 0;
    CodeRange range;
};

using Number = double;
using Character = char;
// Unlike the other atoms Names are just created during parsing.
// After that the std::vector of Names is static, which makes it easier to pass
// around references or pointers.
using Name = std::string;
using NamePointer = const char*;

struct Argument {
    Expression type;
    Expression name;
};

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
    Expression input_name;
    Expression body;
};

struct FunctionBuiltIn {
    std::function<Expression(Expression)> function;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Use index range for names, if names are not internalized.
struct FunctionDictionary {
    Expression environment; // TODO: use this.
    std::vector<Expression> input_names;
    Expression body;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Use index range for names, if names are not internalized.
struct FunctionTuple {
    Expression environment;
    std::vector<Expression> input_names;
    Expression body;
};

struct LookupChild {
    Expression name;
    Expression child;
};

struct FunctionApplication {
    Expression name;
    Expression child;
};

struct LookupSymbol {
    Expression name;
};

struct String {
    Expression top;
    Expression rest;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Cannot just use index range since elements are heterogeneous.
struct Tuple {
    std::vector<Expression> expressions;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Cannot just use index range since elements are heterogeneous.
struct EvaluatedTuple {
    std::vector<Expression> expressions;
};

struct Stack {
    Expression top;
    Expression rest;
};

struct EvaluatedStack {
    Expression top;
    Expression rest;
};

struct Definition {
    Expression name;
    Expression expression;
};

struct PutAssignment {
    Expression name;
    Expression expression;
};

struct PutEachAssignment {
    Expression name;
    Expression expression;
};

struct DropAssignment {
    Expression name;
};

struct WhileStatement {
    Expression expression;
    size_t end_index_;
};

struct ForStatement {
    Expression name_item;
    Expression name_container;
    size_t end_index_;
};

struct ForSimpleStatement {
    Expression name_container;
    size_t end_index_;
};

struct WhileEndStatement {
    size_t while_index_;
};

struct ForEndStatement {
    size_t for_index_;
};

using Statements = std::vector<Expression>;


struct EvaluatedDefinition {
    NamePointer key;
    Expression value;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Bottleneck.
// Add global vector to store all EvaluatedDefinition and use and index range here.
// But then need to know all members of the dictionary before it is evaluated.
// so that they are all added together, at least the first time.
class Definitions {
private:
    std::vector<EvaluatedDefinition> definitions;
public:
    bool empty() const;
    void add(NamePointer key, Expression value);
    bool has(NamePointer key) const;
    Expression lookup(NamePointer key) const;
    std::vector<EvaluatedDefinition> sorted() const;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct Dictionary {
    Statements statements;
};

// TODO: make cheaper to copy or pass by reference or pointer?
// Bottleneck.
struct EvaluatedDictionary {
    Expression environment;
    Definitions definitions;
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
