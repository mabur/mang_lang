#include "serialize.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"
#include "../expressions/LookupChild.h"
#include "../expressions/LookupFunction.h"

std::string serialize(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}

std::string serialize(const Conditional& conditional) {
    return "if " + conditional.expression_if->serialize() +
        " then " + conditional.expression_then->serialize() +
        " else " + conditional.expression_else->serialize();
}

std::string serialize(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& element : dictionary.elements) {
        result += element->serialize();
    }
    if (dictionary.elements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

std::string serialize(const NamedElement& element) {
    return element.name->serialize() + '=' + element.expression->serialize() + ' ';
}

std::string serialize(const WhileElement& element) {
    return "while " + element.expression->serialize() + ' ';
}

std::string serialize(const EndElement&) {
    return "end ";
}

std::string serialize(const Function& function) {
    return "in " + function.input_name->serialize()
        + " out " + function.body->serialize();
}

std::string serialize(const FunctionDictionary& function_dictionary) {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : function_dictionary.input_names) {
        result += name->serialize();
        result += " ";
    }
    if (function_dictionary.input_names.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    result += " out " + function_dictionary.body->serialize();
    return result;
}

std::string serialize(const FunctionList& function_list) {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : function_list.input_names) {
        result += name->serialize();
        result += " ";
    }
    if (function_list.input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " out " + function_list.body->serialize();
    return result;
}

std::string serialize(const List& list) {
    if (!list.list()) {
        return "()";
    }
    const auto operation = [](const std::string& left, const ExpressionPointer& right) {
        return left + right->serialize() + " ";
    };
    auto result = std::string{"("};
    result = leftFold(list.list(), result, operation);
    result.back() = ')';
    return result;
}

std::string serialize(const LookupChild& lookup_child) {
    return lookup_child.name->serialize() + "@" + lookup_child.child->serialize();
}

std::string serialize(const LookupFunction& lookup_function) {
    return lookup_function.name->serialize() + "!" + lookup_function.child->serialize();
}
