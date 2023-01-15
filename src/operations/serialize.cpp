#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"
#include "../container.h"

namespace {

std::string serializeName(Expression name) {
    return getName(name).value;
}

std::string serializeCharacter(const Character& character) {
    return '\'' + std::string{character.value} + '\'';
}

std::string serializeConditional(const Conditional& conditional) {
    return "if " + serialize(conditional.expression_if) +
        " then " + serialize(conditional.expression_then) +
        " else " + serialize(conditional.expression_else);
}

std::string serializeIs(const IsExpression& is_expression) {
    auto result = "is " + serialize(is_expression.input) + " ";
    for (const auto& alternative : is_expression.alternatives) {
        result += serialize(alternative.left);
        result += " then ";
        result += serialize(alternative.right);
        result += " ";
    }
    result += "else ";
    result += serialize(is_expression.expression_else);
    return result;
}

std::string serializeDefinition(const Definition& element) {
    return getName(element.name).value + '=' + serialize(element.expression) + ' ';
}

std::string serializePutAssignment(const PutAssignment& element) {
    return getName(element.name).value + "+=" + serialize(element.expression) + ' ';
}

std::string serializeWhileStatement(const WhileStatement& element) {
    return "while " + serialize(element.expression) + ' ';
}
    
std::string serializeForStatement(const ForStatement& element) {
    return "for " + serialize(element.name_item) + " in " +
        serialize(element.name_container) + " ";
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

std::string serializeEvaluatedDictionary(const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        return "{}";
    }
    auto result = std::string{"{"};
    for (const auto& pair : dictionary.definitions.sorted()) {
        result += pair.first + "=" + serialize(pair.second) + " ";
    }
    result.back() = '}';
    return result;
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

std::string serializeEvaluatedTuple(Expression s) {
    const auto expressions = getEvaluatedTuple(s).expressions;
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

std::string appendElement(const std::string& s, Expression element) {
    return s + serialize(element) + ' ';
}

std::string serializeStack(Expression s) {
    auto result = leftFold(std::string{"["}, s, appendElement, EMPTY_STACK,
        getStack);
    result.back() = ']';
    return result;
}

std::string serializeEvaluatedStack(Expression s) {
    auto result = leftFold(std::string{"["}, s, appendElement, EMPTY_STACK,
        getEvaluatedStack);
    result.back() = ']';
    return result;
}

std::string serializeLookupChild(const LookupChild& lookup_child) {
    return serializeName(lookup_child.name) + "@" + serialize(lookup_child.child);
}

std::string serializeFunctionApplication(const FunctionApplication& function_application) {
    return serializeName(function_application.name) + "!" +
        serialize(function_application.child);
}

std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(lookup_symbol.name);
}

std::string serializeNumber(const Number& number) {
    std::stringstream s;
    s.precision(std::numeric_limits<double>::digits10 + 1);
    s << number.value;
    return s.str();
}

std::string serializeBoolean(const Boolean& boolean) {
    return boolean.value ? "yes" : "no";
}

std::string appendCharacter(const std::string& s, Expression character) {
    return s + getCharacter(character).value;
}

std::string serializeString(Expression string) {
    const auto delimiter = std::string{"\""};
    return leftFold(delimiter, string, appendCharacter, EMPTY_STRING, getString)
        + delimiter;
}

} // namespace

std::string serialize(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(getCharacter(expression));
        case CONDITIONAL: return serializeConditional(getConditional(expression));
        case IS: return serializeIs(getIs(expression));
        case DICTIONARY: return serializeDictionary(getDictionary(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(getPutAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case FUNCTION: return serializeFunction(getFunction(expression));
        case FUNCTION_BUILT_IN: return "built_in_function";
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(getFunctionDictionary(expression));
        case FUNCTION_TUPLE: return serializeFunctionTuple(getFunctionTuple(expression));
        case TABLE: return serializeTable(expression);
        case EVALUATED_TABLE: return serializeEvaluatedTable(expression);
        case TUPLE: return serializeTuple(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(expression);
        case STACK: return serializeStack(expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case EMPTY_STACK: return "[]";
        case LOOKUP_CHILD: return serializeLookupChild(getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case NUMBER: return serializeNumber(getNumber(expression));
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case EMPTY: return "missing";
        default: throw UnexpectedExpression(expression.type, "serialize operation");
    }
}
