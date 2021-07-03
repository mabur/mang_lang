#include "serialize.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"

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
