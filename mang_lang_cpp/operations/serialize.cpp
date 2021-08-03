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

std::string serializeNamedElement(const DictionaryElementPointer& element) {
    return
        serializeName(*element.get()->name) + '=' +
        serialize(element.get()->expression) + ' ';
}

std::string serializeWhileElement(const DictionaryElementPointer& element) {
    return "while " + serialize(element.get()->expression) + ' ';
}

std::string serializeEndElement(const DictionaryElementPointer&) {
    return "end ";
}

std::string serializeDictionaryElement(const DictionaryElementPointer& expression) {
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
    const auto index = expression_smart.index;
    const auto expression = expression_smart.get();
    if (type == CHARACTER) {return serializeCharacter(*dynamic_cast<const Character *>(expression));}
    if (type == CONDITIONAL) {return serializeConditional(*dynamic_cast<const Conditional *>(expression));}
    if (type == DICTIONARY) {return serializeDictionary(*dynamic_cast<const Dictionary *>(expression));}
    if (type == NAMED_ELEMENT) {return serializeDictionaryElement(DictionaryElementPointer{type, index});}
    if (type == WHILE_ELEMENT) {return serializeDictionaryElement(DictionaryElementPointer{type, index});}
    if (type == END_ELEMENT) {return serializeDictionaryElement(DictionaryElementPointer{type, index});}
    if (type == FUNCTION) {return serializeFunction(*dynamic_cast<const Function *>(expression));}
    if (type == FUNCTION_DICTIONARY) {return serializeFunctionDictionary(*dynamic_cast<const FunctionDictionary *>(expression));}
    if (type == FUNCTION_LIST) {return serializeFunctionList(*dynamic_cast<const FunctionList *>(expression));}
    if (type == LIST) {return serializeList(expression_smart);}
    if (type == LOOKUP_CHILD) {return serializeLookupChild(*dynamic_cast<const LookupChild *>(expression));}
    if (type == LOOKUP_FUNCTION) {return serializeLookupFunction(*dynamic_cast<const LookupFunction *>(expression));}
    if (type == LOOKUP_SYMBOL) {return serializeLookupSymbol(*dynamic_cast<const LookupSymbol *>(expression));}
    if (type == NAME) {return serializeName(*dynamic_cast<const Name *>(expression));}
    if (type == NUMBER) {return serializeNumber(*dynamic_cast<const Number *>(expression));}
    if (type == STRING) {return serializeString(expression_smart);}
    throw std::runtime_error{"Did not recognize expression to serializeCharacter: " + std::to_string(type)};
}
