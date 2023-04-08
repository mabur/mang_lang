#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"
#include "../container.h"

namespace {

std::string serializeName(Expression name) {
    return getName(name);
}

template<typename Serializer>
std::string serializeDynamicExpression(
    Serializer serializer, const DynamicExpression& dynamic_expression
) {
    return "dynamic " + serializer(dynamic_expression.expression);
}

template<typename Serializer>
std::string serializeConditional(Serializer serializer, const Conditional& conditional) {
    return "if " + serializer(conditional.expression_if) +
        " then " + serializer(conditional.expression_then) +
        " else " + serializer(conditional.expression_else);
}

template<typename Serializer>
std::string serializeIs(Serializer serializer, const IsExpression& is_expression) {
    auto result = "is " + serializer(is_expression.input) + " ";
    for (const auto& alternative : is_expression.alternatives) {
        result += serializer(alternative.left);
        result += " then ";
        result += serializer(alternative.right);
        result += " ";
    }
    result += "else ";
    result += serializer(is_expression.expression_else);
    return result;
}

template<typename Serializer>
std::string serializeDefinition(Serializer serializer, const Definition& element) {
    return getName(element.name) + '=' + serializer(element.expression) + ' ';
}

template<typename Serializer>
std::string serializePutAssignment(Serializer serializer, const PutAssignment& element) {
    return getName(element.name) + "+=" + serializer(element.expression) + ' ';
}

template<typename Serializer>
std::string serializeWhileStatement(Serializer serializer, const WhileStatement& element) {
    return "while " + serializer(element.expression) + ' ';
}

template<typename Serializer>
std::string serializeForStatement(Serializer serializer, const ForStatement& element) {
    return "for " + serializer(element.name_item) + " in " +
        serializer(element.name_container) + " ";
}

template<typename Serializer>
std::string serializeEvaluatedDictionary(Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        return "{}";
    }
    auto result = std::string{"{"};
    for (const auto& pair : dictionary.definitions.sorted()) {
        result += pair.first + "=" + serializer(pair.second) + " ";
    }
    result.back() = '}';
    return result;
}

template<typename Serializer>
std::string serializeEvaluatedTuple(Serializer serializer, Expression s) {
    const auto expressions = getEvaluatedTuple(s).expressions;
    if (expressions.empty()) {
        return "()";
    }
    auto result = std::string{'('};
    for (const auto& expression : expressions) {
        result += serializer(expression) + ' ';
    }
    result.back() = ')';
    return result;
}

template<typename Serializer>
std::string serializeLookupChild(Serializer serializer, const LookupChild& lookup_child) {
    return serializeName(lookup_child.name) + "@" + serializer(lookup_child.child);
}

template<typename Serializer>
std::string serializeFunctionApplication(Serializer serializer, const FunctionApplication& function_application) {
    return serializeName(function_application.name) + "!" +
        serializer(function_application.child);
}

std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(lookup_symbol.name);
}
    
std::string serializeDictionary(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& statement : dictionary.statements) {
        result += serialize(statement);
    }
    if (dictionary.statements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

std::string serializeTuple(Expression s) {
    const auto expressions = getTuple(s).expressions;
    if (expressions.empty()) {
        return "()";
    }
    auto result = std::string{'('};
    for (const auto& expression : expressions) {
        result += serialize(expression) + ' ';
    }
    result.back() = ')';
    return result;
}

std::string serializeStack(Expression s) {
    const auto appendElement = [=](const std::string& s, Expression element) {
        return s + serialize(element) + ' ';
    };
    auto result = leftFold(
        std::string{"["}, s, appendElement, EMPTY_STACK, getStack
    );
    result.back() = ']';
    return result;
}

std::string serializeCharacter(Character character) {
    return {'\'', character, '\''};
}

std::string serializeFunction(const Function& function) {
    return "in " + serializeName(function.input_name)
        + " out " + serialize(function.body);
}

std::string serializeFunctionDictionary(const FunctionDictionary& function_dictionary) {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : function_dictionary.input_names) {
        result += serializeName(name);
        result += " ";
    }
    if (function_dictionary.input_names.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    result += " out " + serialize(function_dictionary.body);
    return result;
}

std::string serializeFunctionTuple(const FunctionTuple& function_stack) {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : function_stack.input_names) {
        result += serializeName(name);
        result += " ";
    }
    if (function_stack.input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " out " + serialize(function_stack.body);
    return result;
}

std::string serializeTypesTable(Expression s) {
    const auto rows = getTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    const auto& row = *rows.begin();
    return '<' + serialize_types(row.key) + ':' + serialize_types(row.value) + '>';
}
    
std::string serializeTable(Expression s) {
    const auto rows = getTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    auto result = std::string{'<'};
    for (const auto& row : rows) {
        result += serialize(row.key) + ':' + serialize(row.value) + ' ';
    }
    result.back() = '>';
    return result;
}

std::string serializeTypesEvaluatedTable(Expression s) {
    const auto& rows = getEvaluatedTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    const auto& row = rows.begin()->second;
    return '<' + serialize_types(row.key) + ':' + serialize_types(row.value) + '>';
}

std::string serializeEvaluatedTable(Expression s) {
    const auto& rows = getEvaluatedTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    auto result = std::string{'<'};
    for (const auto& row : rows) {
        result += row.first + ':' + serialize(row.second.value) + ' ';
    }
    result.back() = '>';
    return result;
}

std::string serializeTypesEvaluatedStack(Expression s) {
    return '[' + serialize_types(getEvaluatedStack(s).top) + ']';
}
    
std::string appendElement(const std::string& s, Expression element) {
    return s + serialize(element) + ' ';
}

std::string serializeEvaluatedStack(Expression s) {
    auto result = leftFold(std::string{"["}, s, appendElement, EMPTY_STACK,
        getEvaluatedStack);
    result.back() = ']';
    return result;
}

std::string serializeNumber(Number number) {
    std::stringstream s;
    s.precision(std::numeric_limits<double>::digits10 + 1);
    s << number;
    return s.str();
}

std::string serializeBoolean(Boolean boolean) {
    return boolean ? "yes" : "no";
}

std::string appendCharacter(const std::string& s, Expression character) {
    return s + getCharacter(character);
}

std::string serializeString(Expression string) {
    const auto delimiter = std::string{"\""};
    return leftFold(delimiter, string, appendCharacter, EMPTY_STRING, getString)
        + delimiter;
}

} // namespace

std::string serialize_types(Expression expression) {
    // TODO: increase test coverage for all cases.
    // Do we need to add functions for serializing non-evaluated containers?
    // I guess they can occur in function bodies? Or maybe not?
    // In that case can we remove all things that disappear when evaluated?
    switch (expression.type) {
        case CONDITIONAL: return serializeConditional(serialize_types, getConditional(expression));
        case IS: return serializeIs(serialize_types, getIs(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize_types, getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(serialize_types, getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(serialize_types, getPutAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(serialize_types, getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(serialize_types, getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case TABLE: return serializeTypesTable(expression);
        case EVALUATED_TABLE: return serializeTypesEvaluatedTable(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize_types, expression);
        case EVALUATED_STACK: return serializeTypesEvaluatedStack(expression);
        case LOOKUP_CHILD: return serializeLookupChild(serialize_types, getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(serialize_types, getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case DYNAMIC_EXPRESSION: return serializeDynamicExpression(serialize_types, getDynamicExpression(expression));
        default: return NAMES[expression.type];
    }
}

std::string serialize(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(getCharacter(expression));
        case CONDITIONAL: return serializeConditional(serialize, getConditional(expression));
        case IS: return serializeIs(serialize, getIs(expression));
        case DICTIONARY: return serializeDictionary(getDictionary(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize, getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(serialize, getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(serialize, getPutAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(serialize, getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(serialize, getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case FUNCTION: return serializeFunction(getFunction(expression));
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(getFunctionDictionary(expression));
        case FUNCTION_TUPLE: return serializeFunctionTuple(getFunctionTuple(expression));
        case TABLE: return serializeTable(expression);
        case EVALUATED_TABLE: return serializeEvaluatedTable(expression);
        case TUPLE: return serializeTuple(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize, expression);
        case STACK: return serializeStack(expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case EMPTY_STACK: return "[]";
        case LOOKUP_CHILD: return serializeLookupChild(serialize, getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(serialize, getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case NUMBER: return serializeNumber(getNumber(expression));
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case DYNAMIC_EXPRESSION: return serializeDynamicExpression(serialize, getDynamicExpression(expression));
        default: return NAMES[expression.type];
    }
}
