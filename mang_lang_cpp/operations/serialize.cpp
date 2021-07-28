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

std::string serialize(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}

std::string serialize(const Conditional& conditional) {
    return "if " + serialize(conditional.expression_if) +
        " then " + serialize(conditional.expression_then) +
        " else " + serialize(conditional.expression_else);
}

std::string serialize(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& element : dictionary.elements) {
        result += serialize(element);
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
    return serialize(element.name) + '=' + serialize(element.expression) + ' ';
}

std::string serialize(const WhileElement& element) {
    return "while " + serialize(element.expression) + ' ';
}

std::string serialize(const EndElement&) {
    return "end ";
}

std::string serialize(const Function& function) {
    return "in " + serialize(function.input_name)
        + " out " + serialize(function.body);
}

std::string serialize(const FunctionDictionary& function_dictionary) {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : function_dictionary.input_names) {
        result += serialize(name);
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

std::string serialize(const FunctionList& function_list) {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : function_list.input_names) {
        result += serialize(name);
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

std::string serialize_list(const ExpressionPointer& list) {
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

std::string serialize(const LookupChild& lookup_child) {
    return serialize(lookup_child.name) + "@" + serialize(lookup_child.child);
}

std::string serialize(const LookupFunction& lookup_function) {
    return serialize(lookup_function.name) + "!" + serialize(lookup_function.child);
}

std::string serialize(const LookupSymbol& lookup_symbol) {
    return serialize(lookup_symbol.name);
}

std::string serialize(const Name& name) {
    return name.value;
}

std::string serialize(const Number& number) {
    std::stringstream s;
    s << number.value;
    return s.str();
}

std::string serialize_string(const ExpressionPointer& string) {
    auto value = std::string{"\""};
    auto node = list(string);
    for (; node; node = node->rest) {
        value += character(node->first);
    }
    value += "\"";
    return value;
}

std::string serialize(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == CHARACTER) {return serialize(*dynamic_cast<const Character*>(expression));}
    if (expression->type_ == CONDITIONAL) {return serialize(*dynamic_cast<const Conditional*>(expression));}
    if (expression->type_ == DICTIONARY) {return serialize(*dynamic_cast<const Dictionary*>(expression));}
    if (expression->type_ == NAMED_ELEMENT) {return serialize(*dynamic_cast<const NamedElement*>(expression));}
    if (expression->type_ == WHILE_ELEMENT) {return serialize(*dynamic_cast<const WhileElement*>(expression));}
    if (expression->type_ == END_ELEMENT) {return serialize(*dynamic_cast<const EndElement*>(expression));}
    if (expression->type_ == FUNCTION) {return serialize(*dynamic_cast<const Function*>(expression));}
    if (expression->type_ == FUNCTION_DICTIONARY) {return serialize(*dynamic_cast<const FunctionDictionary*>(expression));}
    if (expression->type_ == FUNCTION_LIST) {return serialize(*dynamic_cast<const FunctionList*>(expression));}
    if (expression->type_ == LIST) {return serialize_list(expression_smart);}
    if (expression->type_ == LOOKUP_CHILD) {return serialize(*dynamic_cast<const LookupChild*>(expression));}
    if (expression->type_ == LOOKUP_FUNCTION) {return serialize(*dynamic_cast<const LookupFunction*>(expression));}
    if (expression->type_ == LOOKUP_SYMBOL) {return serialize(*dynamic_cast<const LookupSymbol*>(expression));}
    if (expression->type_ == NAME) {return serialize(*dynamic_cast<const Name*>(expression));}
    if (expression->type_ == NUMBER) {return serialize(*dynamic_cast<const Number*>(expression));}
    if (expression->type_ == STRING) {return serialize_string(expression_smart);}
    throw std::runtime_error{"Did not recognize expression to serialize: " + std::to_string(expression->type_)};
}
