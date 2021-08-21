#include "Expression.h"

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

NamedElement getNamedElement(ExpressionPointer expression) {
    assert(expression.type == NAMED_ELEMENT);
    return *named_elements.at(expression.index).get();
}

WhileElement getWileElement(ExpressionPointer expression) {
    assert(expression.type == WHILE_ELEMENT);
    return *while_elements.at(expression.index).get();
}

EndElement getEndElement(ExpressionPointer expression) {
    assert(expression.type == END_ELEMENT);
    return *end_elements.at(expression.index).get();
}

Number getNumber(ExpressionPointer expression) {
    assert(expression.type == NUMBER);
    return *numbers.at(expression.index).get();
}

Character getCharacter(ExpressionPointer expression) {
    assert(expression.type == CHARACTER);
    return *characters.at(expression.index).get();
}

Conditional getConditional(ExpressionPointer expression) {
    assert(expression.type == CONDITIONAL);
    return *conditionals.at(expression.index).get();
}

Dictionary getDictionary(ExpressionPointer expression) {
    assert(expression.type == DICTIONARY);
    return *dictionaries.at(expression.index).get();
}

Function getFunction(ExpressionPointer expression) {
    assert(expression.type == FUNCTION);
    return *functions.at(expression.index).get();
}

FunctionBuiltIn getFunctionBuiltIn(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_BUILT_IN);
    return *built_in_functions.at(expression.index).get();
}

FunctionDictionary getFunctionDictionary(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_DICTIONARY);
    return *dictionary_functions.at(expression.index).get();
}

FunctionList getFunctionList(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_LIST);
    return *list_functions.at(expression.index).get();
}

List getList(ExpressionPointer expression) {
    assert(expression.type == LIST);
    return *lists.at(expression.index).get();
}

LookupChild getLokupChild(ExpressionPointer expression) {
    assert(expression.type == LOOKUP_CHILD);
    return *child_lookups.at(expression.index).get();
}

FunctionApplication getFunctionApplication(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_APPLICATION);
    return *function_applications.at(expression.index).get();
}

LookupSymbol getLookupSymbol(ExpressionPointer expression) {
    assert(expression.type == LOOKUP_SYMBOL);
    return *symbol_lookups.at(expression.index).get();
}

Name getName(ExpressionPointer expression) {
    assert(expression.type == NAME);
    return *names.at(expression.index).get();
}

String getString(ExpressionPointer expression) {
    assert(expression.type == STRING);
    return *strings.at(expression.index).get();
}
