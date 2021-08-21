#include "factory.h"

#include <cassert>
#include <vector>

#include "operations/begin.h"

std::vector<std::shared_ptr<const Character>> characters;
std::vector<std::shared_ptr<const Conditional>> conditionals;
std::vector<std::shared_ptr<const Dictionary>> dictionaries;
std::vector<std::shared_ptr<const Function>> functions;
std::vector<std::shared_ptr<const FunctionBuiltIn>> built_in_functions;
std::vector<std::shared_ptr<const FunctionDictionary>> dictionary_functions;
std::vector<std::shared_ptr<const FunctionList>> list_functions;
std::vector<std::shared_ptr<const List>> lists;
std::vector<std::shared_ptr<const LookupChild>> child_lookups;
std::vector<std::shared_ptr<const FunctionApplication>> function_applications;
std::vector<std::shared_ptr<const LookupSymbol>> symbol_lookups;
std::vector<std::shared_ptr<const Name>> names;
std::vector<std::shared_ptr<const Number>> numbers;
std::vector<std::shared_ptr<const String>> strings;
std::vector<std::shared_ptr<const WhileElement>> while_elements;
std::vector<std::shared_ptr<const EndElement>> end_elements;
std::vector<std::shared_ptr<const NamedElement>> named_elements;
std::vector<ExpressionPointer> expressions;

void clearMemory() {
    characters.clear();
    conditionals.clear();
    dictionaries.clear();
    functions.clear();
    built_in_functions.clear();
    dictionary_functions.clear();
    list_functions.clear();
    lists.clear();
    child_lookups.clear();
    function_applications.clear();
    symbol_lookups.clear();
    names.clear();
    numbers.clear();
    strings.clear();
    while_elements.clear();
    end_elements.clear();
    named_elements.clear();
    expressions.clear();
}

std::vector<size_t> whileIndices(const DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    auto while_indices = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        const auto type = elements[i].type;
        if (type == NAMED_ELEMENT) {
            while_indices.push_back(1); // dummy
        }
        if (type == WHILE_ELEMENT) {
            while_positions.push_back(i);
            while_indices.push_back(1); // dummy
        }
        if (type == END_ELEMENT) {
            while_indices.push_back(while_positions.back());
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", begin(elements.front()));
    }
    return while_indices;
}

std::vector<size_t> endIndices(const DictionaryElements& elements) {
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    auto end_indices = std::vector<size_t>(elements.size());
    for (size_t i = elements.size() - 1; i < elements.size(); --i) {
        const auto type = elements[i].type;
        if (type == NAMED_ELEMENT) {
            end_indices[i] = 0; // dummy
        }
        if (type == WHILE_ELEMENT) {
            end_indices[i] = end_positions.back();
            end_positions.pop_back();
        }
        if (type == END_ELEMENT) {
            end_indices[i] = 0; // dummy
            end_positions.push_back(i);
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", begin(elements.front()));
    }
    return end_indices;
}

std::vector<size_t> nameIndices(const DictionaryElements& elements) {
    // Forward pass to set name_index_:
    auto names = std::vector<std::string>{};
    auto name_indices = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        const auto type = elements[i].type;
        if (type == NAMED_ELEMENT) {
            const auto name = getNamedElement(elements[i]).name->value;
            const auto it = std::find(names.begin(), names.end(), name);
            name_indices.push_back(std::distance(names.begin(), it));
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (type == WHILE_ELEMENT) {
            name_indices.push_back(0); // dummy
        }
        if (type == END_ELEMENT) {
            name_indices.push_back(0); // dummy
        }
    }
    return name_indices;
}

DictionaryElements setContext(const DictionaryElements& elements) {
    const auto while_indices = whileIndices(elements);
    const auto end_indices = endIndices(elements);
    const auto name_indices = nameIndices(elements);

    auto result = DictionaryElements{};

    for (size_t i = 0; i < elements.size(); ++i) {
        const auto type = elements[i].type;
        if (type == NAMED_ELEMENT) {
            const auto element = getNamedElement(elements[i]);
            result.push_back(makeNamedElement(new NamedElement{
                element.range,
                element.name,
                element.expression,
                name_indices[i]
            }));
        } else if (type == WHILE_ELEMENT) {
            const auto element = getWileElement(elements[i]);
            result.push_back(makeWhileElement(new WhileElement{
                element.range,
                element.expression,
                end_indices[i],
            }));
        } else if (type == END_ELEMENT) {
            const auto element = getEndElement(elements[i]);
            result.push_back(makeEndElement(new EndElement{
                element.range, while_indices[i]
            }));
        } else {
            assert("Unexpected type");
        }
    }
    return result;
}

template<typename ElementType, typename ArrayType>
ExpressionPointer makeExpression(
    const ElementType* expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(ExpressionPointer{type, array.size() - 1});
    return expressions.back();
}

ExpressionPointer makeNumber(const Number* expression) {
    return makeExpression(expression, NUMBER, numbers);
}

ExpressionPointer makeCharacter(const Character* expression) {
    return makeExpression(expression, CHARACTER, characters);
}

ExpressionPointer makeConditional(const Conditional* expression) {
    return makeExpression(expression, CONDITIONAL, conditionals);
}

ExpressionPointer makeDictionary(const Dictionary* expression) {
    return makeExpression(expression, DICTIONARY, dictionaries);
}

ExpressionPointer makeFunction(const Function* expression) {
    return makeExpression(expression, FUNCTION, functions);
}

ExpressionPointer makeFunctionBuiltIn(const FunctionBuiltIn* expression) {
    return makeExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

ExpressionPointer makeFunctionDictionary(const FunctionDictionary* expression) {
    return makeExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

ExpressionPointer makeFunctionList(const FunctionList* expression) {
    return makeExpression(expression, FUNCTION_LIST, list_functions);
}

ExpressionPointer makeList(const List* expression) {
    return makeExpression(expression, LIST, lists);
}

ExpressionPointer makeLookupChild(const LookupChild* expression) {
    return makeExpression(expression, LOOKUP_CHILD, child_lookups);
}

ExpressionPointer makeFunctionApplication(const FunctionApplication* expression) {
    return makeExpression(expression, FUNCTION_APPLICATION, function_applications);
}

ExpressionPointer makeLookupSymbol(const LookupSymbol* expression) {
    return makeExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

ExpressionPointer makeName(const Name* expression) {
    return makeExpression(expression, NAME, names);
}

ExpressionPointer makeString(const String* expression) {
    return makeExpression(expression, STRING, strings);
}

ExpressionPointer makeNamedElement(const NamedElement* expression) {
    return makeExpression(expression, NAMED_ELEMENT, named_elements);
}

ExpressionPointer makeWhileElement(const WhileElement* expression) {
    return makeExpression(expression, WHILE_ELEMENT, while_elements);
}

ExpressionPointer makeEndElement(const EndElement* expression) {
    return makeExpression(expression, END_ELEMENT, end_elements);
}

// FREE GETTERS

template<typename ArrayType>
typename ArrayType::value_type::element_type getExpression(
    ExpressionPointer expression,
    ExpressionType type,
    ArrayType& array
) {
    assert(expression.type == type);
    return *array.at(expression.index).get();
}

NamedElement getNamedElement(ExpressionPointer expression) {
    return getExpression(expression, NAMED_ELEMENT, named_elements);
}

WhileElement getWileElement(ExpressionPointer expression) {
    return getExpression(expression, WHILE_ELEMENT, while_elements);
}

EndElement getEndElement(ExpressionPointer expression) {
    return getExpression(expression, END_ELEMENT, end_elements);
}

Number getNumber(ExpressionPointer expression) {
    return getExpression(expression, NUMBER, numbers);
}

Character getCharacter(ExpressionPointer expression) {
    return getExpression(expression, CHARACTER, characters);
}

Conditional getConditional(ExpressionPointer expression) {
    return getExpression(expression, CONDITIONAL, conditionals);
}

Dictionary getDictionary(ExpressionPointer expression) {
    return getExpression(expression, DICTIONARY, dictionaries);
}

Function getFunction(ExpressionPointer expression) {
    return getExpression(expression, FUNCTION, functions);
}

FunctionBuiltIn getFunctionBuiltIn(ExpressionPointer expression) {
    return getExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

FunctionDictionary getFunctionDictionary(ExpressionPointer expression) {
    return getExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

FunctionList getFunctionList(ExpressionPointer expression) {
    return getExpression(expression, FUNCTION_LIST, list_functions);
}

List getList(ExpressionPointer expression) {
    return getExpression(expression, LIST, lists);
}

LookupChild getLokupChild(ExpressionPointer expression) {
    return getExpression(expression, LOOKUP_CHILD, child_lookups);
}

FunctionApplication getFunctionApplication(ExpressionPointer expression) {
    return getExpression(expression, FUNCTION_APPLICATION, function_applications);
}

LookupSymbol getLookupSymbol(ExpressionPointer expression) {
    return getExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Name getName(ExpressionPointer expression) {
    return getExpression(expression, NAME, names);
}

String getString(ExpressionPointer expression) {
    return getExpression(expression, STRING, strings);
}
