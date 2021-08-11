#include "serialize.h"

#include "character.h"
#include "list.h"

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

std::string serializeNamedElement(ExpressionPointer element) {
    return
        serializeName(*element.dictionaryElement().name) + '=' +
        serialize(element.dictionaryElement().expression) + ' ';
}

std::string serializeWhileElement(ExpressionPointer element) {
    return "while " + serialize(element.dictionaryElement().expression) + ' ';
}

std::string serializeEndElement(ExpressionPointer) {
    return "end ";
}

std::string serializeDictionaryElement(ExpressionPointer expression) {
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

std::string serializeList(ExpressionPointer list) {
    if (!::list(list)) {
        return "()";
    }
    const auto operation = [](const std::string& left, ExpressionPointer right) {
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

std::string serializeString(ExpressionPointer string) {
    auto value = std::string{"\""};
    auto node = list(string);
    for (; node; node = node->rest) {
        value += character(node->first);
    }
    value += "\"";
    return value;
}

std::string serialize(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(expression.character());
        case CONDITIONAL: return serializeConditional(expression.conditional());
        case DICTIONARY: return serializeDictionary(expression.dictionary());
        case NAMED_ELEMENT: return serializeDictionaryElement(expression);
        case WHILE_ELEMENT: return serializeDictionaryElement(expression);
        case END_ELEMENT: return serializeDictionaryElement(expression);
        case FUNCTION: return serializeFunction(expression.function());
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(expression.functionDictionary());
        case FUNCTION_LIST: return serializeFunctionList(expression.functionList());
        case LIST: return serializeList(expression);
        case LOOKUP_CHILD: return serializeLookupChild(expression.lookupChild());
        case LOOKUP_FUNCTION: return serializeLookupFunction(expression.lookupFunction());
        case LOOKUP_SYMBOL: return serializeLookupSymbol(expression.lookupSymbol());
        case NAME: return serializeName(expression.name());
        case NUMBER: return serializeNumber(expression.number());
        case STRING: return serializeString(expression);
        default: throw std::runtime_error{"Did not recognize expression to serializeCharacter: " + std::to_string(expression.type)};
    }
}
