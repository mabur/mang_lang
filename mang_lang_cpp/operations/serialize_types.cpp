#include "serialize_types.h"

#include <sstream>

#include "../factory.h"
#include "../container.h"

namespace {

std::string serializeName(Expression name) {
    return getName(name).value;
}

std::string serializeLabel(Expression name) {
    return "\'" + getLabel(name).value + "\'";
}

std::string serializeConditional(const Conditional& conditional) {
    return "if " + serialize_types(conditional.expression_if) +
        " then " + serialize_types(conditional.expression_then) +
        " else " + serialize_types(conditional.expression_else);
}

std::string serializeIs(const IsExpression& is_expression) {
    auto result = "is " + serialize_types(is_expression.input) + " ";
    for (const auto& alternative : is_expression.alternatives) {
        result += serialize_types(alternative.left);
        result += " then ";
        result += serialize_types(alternative.right);
        result += " ";
    }
    result += "else ";
    result += serialize_types(is_expression.expression_else);
    return result;
}

std::string serializeDefinition(const Definition& element) {
    return getName(element.name).value + '=' + serialize_types(element.expression) + ' ';
}

std::string serializeDynamicDefinition(const DynamicDefinition& element) {
    return '<' + serialize_types(element.dynamic_name) + '>' + '=' +
        serialize_types(element.expression) + ' ';
}

std::string serializeWhileStatement(const WhileStatement& element) {
    return "while " + serialize_types(element.expression) + ' ';
}

std::string serializeEndStatement(const EndStatement&) {
    return "end ";
}

std::string serializeDictionary(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& statement : dictionary.statements) {
        result += serialize_types(statement);
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
        if (c == '\'') {
            result += pair.first.substr(1, pair.first.size() - 2) + "=" + serialize_types(pair.second) + " ";
        } else {
            result += "<" + pair.first + ">" + "=" + serialize_types(pair.second) + " ";
        }
    }
    result.back() = '}';
    return result;
}

std::string serializeFunction(const Function& function) {
    return "in " + serializeName(function.input_name)
        + " out " + serialize_types(function.body);
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
    result += " out " + serialize_types(function_dictionary.body);
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
    result += " out " + serialize_types(function_stack.body);
    return result;
}

std::string serializeTuple(Expression s) {
    const auto expressions = getTuple(s).expressions;
    if (expressions.empty()) {
        return "()";
    }
    auto result = std::string{'('};
    for (const auto& expression : expressions) {
        result += serialize_types(expression) + ' ';
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
        result += serialize_types(expression) + ' ';
    }
    result.back() = ')';
    return result;
}

std::string appendElement(const std::string& s, Expression element) {
    return s + serialize_types(element) + ' ';
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
    return serializeName(lookup_child.name) + "@" + serialize_types(lookup_child.child);
}

std::string serializeFunctionApplication(const FunctionApplication& function_application) {
    return serializeName(function_application.name) + "!" +
        serialize_types(function_application.child);
}

std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(lookup_symbol.name);
}

std::string serializeDynamicLookupSymbol(const DynamicLookupSymbol& dynamic_lookup_symbol) {
    return "<" + serialize_types(dynamic_lookup_symbol.expression) + ">";
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

std::string serialize_types(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return NAMES[CHARACTER];
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
        case FUNCTION_TUPLE: return serializeFunctionTuple(getFunctionTuple(expression));
        case TUPLE: return serializeTuple(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(expression);
        case STACK: return serializeStack(expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case EMPTY_STACK: return "[]";
        case LOOKUP_CHILD: return serializeLookupChild(getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case DYNAMIC_LOOKUP_SYMBOL: return serializeDynamicLookupSymbol(getDynamicLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case LABEL: return serializeLabel(expression);
        case NUMBER: return NAMES[NUMBER];
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case EMPTY: return "missing";
        default: throw UnexpectedExpression(expression.type, "serialize operation");
    }
}
