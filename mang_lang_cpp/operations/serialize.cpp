#include "serialize.h"

#include <sstream>

#include "../factory.h"
#include "boolean.h"
#include "../container.h"

std::string serializeName(Expression name) {
    return getName(name).value;
}

std::string serializeCharacter(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}

std::string serializeConditional(const Conditional& conditional) {
    return "if " + serialize(conditional.expression_if) +
        " then " + serialize(conditional.expression_then) +
        " else " + serialize(conditional.expression_else);
}

std::string serializeDefinition(const Definition& element) {
    return serializeName(element.name) + '=' + serialize(element.expression) + ' ';
}

std::string serializeWhileStatement(const WhileStatement& element) {
    return "while " + serialize(element.expression) + ' ';
}

std::string serializeEndStatement(const EndStatement&) {
    return "end ";
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

std::string serializeFunctionList(const FunctionList& function_list) {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : function_list.input_names) {
        result += serializeName(name);
        result += " ";
    }
    if (function_list.input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " out " + serialize(function_list.body);
    return result;
}

std::string appendElement(const std::string& s, Expression element) {
    return s + serialize(element) + ' ';
}

std::string serializeList(Expression s) {
    auto result = new_list::leftFold(std::string{"("}, s, appendElement);
    result.back() = ')';
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
    return new_string::leftFold(delimiter, string, appendCharacter) + delimiter;
}

std::string serialize(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(getCharacter(expression));
        case CONDITIONAL: return serializeConditional(getConditional(expression));
        case DICTIONARY: return serializeDictionary(getDictionary(expression));
        case DEFINITION: return serializeDefinition(getDefinition(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(getWileStatement(expression));
        case END_STATEMENT: return serializeEndStatement(getEndStatement(expression));
        case FUNCTION: return serializeFunction(getFunction(expression));
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(getFunctionDictionary(expression));
        case FUNCTION_LIST: return serializeFunctionList(getFunctionList(expression));
        case LIST: return serializeList(expression);
        case EMPTY_LIST: return "()";
        case LOOKUP_CHILD: return serializeLookupChild(getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case NUMBER: return serializeNumber(getNumber(expression));
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case EMPTY: return "DUMMY_VALUE";
        default: throw std::runtime_error{"Did not recognize expression to serialize: " + std::to_string(expression.type)};
    }
}
