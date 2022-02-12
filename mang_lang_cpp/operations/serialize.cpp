#include "serialize.h"

#include <sstream>

#include "../factory.h"
#include "../container.h"

std::string serializeName(Expression name) {
    return getName(name).value;
}

std::string serializeCharacter(const Character& character) {
    return "\\" + std::string{character.value};
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

std::string serializeDynamicDefinition(const DynamicDefinition& element) {
    return '[' + serialize(element.dynamic_name) + ']' + '=' +
        serialize(element.expression) + ' ';
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

std::string serializeEvaluatedDictionary(const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        return "{}";
    }
    auto result = std::string{"{"};
    for (const auto& pair : dictionary.definitions.sorted()) {
        const auto c = pair.first[0];
        if (std::isalpha(c) or c == '_') {
            result += pair.first + "=" + serialize(pair.second) + " ";
        } else {
            result += "[" + pair.first + "]" + "=" + serialize(pair.second) + " ";
        }
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
    auto result = leftFold(std::string{"("}, s, appendElement, EMPTY_LIST, getList);
    result.back() = ')';
    return result;
}

std::string serializeEvaluatedList(Expression s) {
    auto result = leftFold(std::string{"("}, s, appendElement, EMPTY_LIST, getEvaluatedList);
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

std::string serializeDynamicLookupSymbol(const DynamicLookupSymbol& dynamic_lookup_symbol) {
    return "[" + serialize(dynamic_lookup_symbol.expression) + "]";
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

std::string serialize(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(getCharacter(expression));
        case CONDITIONAL: return serializeConditional(getConditional(expression));
        case IS: return serializeIs(getIs(expression));
        case DICTIONARY: return serializeDictionary(getDictionary(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(getDefinition(expression));
        case DYNAMIC_DEFINITION: return serializeDynamicDefinition(getDynamicDefinition(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(getWileStatement(expression));
        case END_STATEMENT: return serializeEndStatement(getEndStatement(expression));
        case FUNCTION: return serializeFunction(getFunction(expression));
        case FUNCTION_BUILT_IN: return "built_in_function";
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(getFunctionDictionary(expression));
        case FUNCTION_LIST: return serializeFunctionList(getFunctionList(expression));
        case LIST: return serializeList(expression);
        case EVALUATED_LIST: return serializeEvaluatedList(expression);
        case EMPTY_LIST: return "()";
        case LOOKUP_CHILD: return serializeLookupChild(getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case DYNAMIC_LOOKUP_SYMBOL: return serializeDynamicLookupSymbol(getDynamicLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case NUMBER: return serializeNumber(getNumber(expression));
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case EMPTY: return "DUMMY_VALUE";
        default: throw UnexpectedExpression(expression.type, "serialize operation");
    }
}
