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
#include "../expressions/LookupSymbol.h"
#include "../expressions/Name.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

#include "character.h"
#include "list.h"

#include "../factory.h"

std::string serializeName(const Name& name) {
    return name.value;
}

std::string serializeCharacter(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}

std::string serializeConditional(const Conditional& conditional) {
    return "if " + serialize(conditional.expression_if) +
        " then " + serialize(conditional.expression_then) +
        " else " + serialize(conditional.expression_else);
}

std::string serializeNamedElement(const ExpressionPointer& element) {
    return
        serializeName(*element.dictionaryElement().name) + '=' +
        serialize(element.dictionaryElement().expression) + ' ';
}

std::string serializeWhileElement(const ExpressionPointer& element) {
    return "while " + serialize(element.dictionaryElement().expression) + ' ';
}

std::string serializeEndElement(const ExpressionPointer&) {
    return "end ";
}

std::string serializeDictionaryElement(const ExpressionPointer& expression) {
    if (expression.type == NAMED_ELEMENT) {return serializeNamedElement(expression);}
    if (expression.type == WHILE_ELEMENT) {return serializeWhileElement(expression);}
    if (expression.type == END_ELEMENT) {return serializeEndElement(expression);}
    throw std::runtime_error{"Did not recognize expression to serializeCharacter: " + std::to_string(expression.type)};
}

std::string serializeDictionary(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& element : dictionary.elements) {
        result += serializeDictionaryElement(element);
    }
    if (dictionary.elements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

std::string serializeFunction(const Function& function) {
    return "in " + serializeName(*function.input_name)
        + " out " + serialize(function.body);
}

std::string serializeFunctionDictionary(const FunctionDictionary& function_dictionary) {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : function_dictionary.input_names) {
        result += serializeName(*name);
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
        result += serializeName(*name);
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

std::string serializeList(const ExpressionPointer& list) {
    if (!::list(list)) {
        return "()";
    }
    const auto operation = [](const std::string& left, const ExpressionPointer& right) {
        return left + serialize(right) + " ";
    };
    auto result = std::string{"("};
    result = leftFold(::list(list), result, operation);
    result.back() = ')';
    return result;
}

std::string serializeLookupChild(const LookupChild& lookup_child) {
    return serializeName(*lookup_child.name) + "@" + serialize(lookup_child.child);
}

std::string serializeLookupFunction(const LookupFunction& lookup_function) {
    return serializeName(*lookup_function.name) + "!" + serialize(lookup_function.child);
}

std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(*lookup_symbol.name);
}

std::string serializeNumber(const Number& number) {
    std::stringstream s;
    s << number.value;
    return s.str();
}

std::string serializeString(const ExpressionPointer& string) {
    auto value = std::string{"\""};
    auto node = list(string);
    for (; node; node = node->rest) {
        value += character(node->first);
    }
    value += "\"";
    return value;
}

std::string serialize(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    switch (type) {
        case CHARACTER: return serializeCharacter(expression_smart.character());
        case CONDITIONAL: return serializeConditional(expression_smart.conditional());
        case DICTIONARY: return serializeDictionary(expression_smart.dictionary());
        case NAMED_ELEMENT: return serializeDictionaryElement(expression_smart);
        case WHILE_ELEMENT: return serializeDictionaryElement(expression_smart);
        case END_ELEMENT: return serializeDictionaryElement(expression_smart);
        case FUNCTION: return serializeFunction(expression_smart.function());
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(expression_smart.functionDictionary());
        case FUNCTION_LIST: return serializeFunctionList(expression_smart.functionList());
        case LIST: return serializeList(expression_smart);
        case LOOKUP_CHILD: return serializeLookupChild(expression_smart.lookupChild());
        case LOOKUP_FUNCTION: return serializeLookupFunction(expression_smart.lookupFunction());
        case LOOKUP_SYMBOL: return serializeLookupSymbol(expression_smart.lookupSymbol());
        case NAME: return serializeName(expression_smart.name());
        case NUMBER: return serializeNumber(expression_smart.number());
        case STRING: return serializeString(expression_smart);
        default: throw std::runtime_error{"Did not recognize expression to serializeCharacter: " + std::to_string(type)};
    }
}
