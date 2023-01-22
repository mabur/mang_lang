#pragma once

#include <string>
#include "../factory.h"

inline 
std::string serializeName(Expression name) {
    return getName(name).value;
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
    return getName(element.name).value + '=' + serializer(element.expression) + ' ';
}

template<typename Serializer>
std::string serializePutAssignment(Serializer serializer, const PutAssignment& element) {
    return getName(element.name).value + "+=" + serializer(element.expression) + ' ';
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

inline
std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(lookup_symbol.name);
}
