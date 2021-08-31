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
std::vector<std::shared_ptr<const WhileStatement>> while_statements;
std::vector<std::shared_ptr<const EndElement>> end_elements;
std::vector<std::shared_ptr<const Definition>> definitions;
std::vector<std::shared_ptr<const String>> new_strings;
std::vector<std::shared_ptr<const EmptyString>> new_empty_strings;
std::vector<Expression> expressions;

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
    while_statements.clear();
    end_elements.clear();
    definitions.clear();
    new_strings.clear();
    expressions.clear();
}

std::vector<size_t> whileIndices(const DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    auto while_indices = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        const auto type = elements[i].type;
        if (type == DEFINITION) {
            while_indices.push_back(1); // dummy
        }
        if (type == WHILE_STATEMENT) {
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
        if (type == DEFINITION) {
            end_indices[i] = 0; // dummy
        }
        if (type == WHILE_STATEMENT) {
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
        if (type == DEFINITION) {
            const auto name = getName(getDefinition(elements[i]).name).value;
            const auto it = std::find(names.begin(), names.end(), name);
            name_indices.push_back(std::distance(names.begin(), it));
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (type == WHILE_STATEMENT) {
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
        if (type == DEFINITION) {
            const auto element = getDefinition(elements[i]);
            result.push_back(makeDefinition(new Definition{
                element.range,
                element.name,
                element.expression,
                name_indices[i]
            }));
        } else if (type == WHILE_STATEMENT) {
            const auto element = getWileStatement(elements[i]);
            result.push_back(makeWhileStatement(new WhileStatement{
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
Expression makeExpression(
    const ElementType* expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1});
    return expressions.back();
}

Expression makeNumber(const Number* expression) {
    return makeExpression(expression, NUMBER, numbers);
}

Expression makeCharacter(const Character* expression) {
    return makeExpression(expression, CHARACTER, characters);
}

Expression makeConditional(const Conditional* expression) {
    return makeExpression(expression, CONDITIONAL, conditionals);
}

Expression makeDictionary(const Dictionary* expression) {
    return makeExpression(expression, DICTIONARY, dictionaries);
}

Expression makeFunction(const Function* expression) {
    return makeExpression(expression, FUNCTION, functions);
}

Expression makeFunctionBuiltIn(const FunctionBuiltIn* expression) {
    return makeExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

Expression makeFunctionDictionary(const FunctionDictionary* expression) {
    return makeExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

Expression makeFunctionList(const FunctionList* expression) {
    return makeExpression(expression, FUNCTION_LIST, list_functions);
}

Expression makeList(const List* expression) {
    return makeExpression(expression, LIST, lists);
}

Expression makeLookupChild(const LookupChild* expression) {
    return makeExpression(expression, LOOKUP_CHILD, child_lookups);
}

Expression makeFunctionApplication(const FunctionApplication* expression) {
    return makeExpression(expression, FUNCTION_APPLICATION, function_applications);
}

Expression makeLookupSymbol(const LookupSymbol* expression) {
    return makeExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Expression makeName(const Name* expression) {
    return makeExpression(expression, NAME, names);
}

Expression makeDefinition(const Definition* expression) {
    return makeExpression(expression, DEFINITION, definitions);
}

Expression makeWhileStatement(const WhileStatement* expression) {
    return makeExpression(expression, WHILE_STATEMENT, while_statements);
}

Expression makeEndElement(const EndElement* expression) {
    return makeExpression(expression, END_ELEMENT, end_elements);
}

Expression makeNewString(const String* expression) {
    return makeExpression(expression, NEW_STRING, new_strings);
}

Expression makeNewEmptyString(const EmptyString* expression) {
    return makeExpression(expression, NEW_EMPTY_STRING, new_empty_strings);
}

// FREE GETTERS

template<typename ArrayType>
typename ArrayType::value_type::element_type getExpression(
    Expression expression,
    ExpressionType type,
    ArrayType& array
) {
    assert(expression.type == type);
    return *array.at(expression.index).get();
}

Definition getDefinition(Expression expression) {
    return getExpression(expression, DEFINITION, definitions);
}

WhileStatement getWileStatement(Expression expression) {
    return getExpression(expression, WHILE_STATEMENT, while_statements);
}

EndElement getEndElement(Expression expression) {
    return getExpression(expression, END_ELEMENT, end_elements);
}

Number getNumber(Expression expression) {
    return getExpression(expression, NUMBER, numbers);
}

Character getCharacter(Expression expression) {
    return getExpression(expression, CHARACTER, characters);
}

Conditional getConditional(Expression expression) {
    return getExpression(expression, CONDITIONAL, conditionals);
}

Dictionary getDictionary(Expression expression) {
    return getExpression(expression, DICTIONARY, dictionaries);
}

Function getFunction(Expression expression) {
    return getExpression(expression, FUNCTION, functions);
}

FunctionBuiltIn getFunctionBuiltIn(Expression expression) {
    return getExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

FunctionDictionary getFunctionDictionary(Expression expression) {
    return getExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

FunctionList getFunctionList(Expression expression) {
    return getExpression(expression, FUNCTION_LIST, list_functions);
}

List getList(Expression expression) {
    return getExpression(expression, LIST, lists);
}

LookupChild getLokupChild(Expression expression) {
    return getExpression(expression, LOOKUP_CHILD, child_lookups);
}

FunctionApplication getFunctionApplication(Expression expression) {
    return getExpression(expression, FUNCTION_APPLICATION, function_applications);
}

LookupSymbol getLookupSymbol(Expression expression) {
    return getExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Name getName(Expression expression) {
    return getExpression(expression, NAME, names);
}

String getNewString(Expression expression) {
    return getExpression(expression, NEW_STRING, new_strings);
}

EmptyString getNewEmptyString(Expression expression) {
    return getExpression(expression, NEW_EMPTY_STRING, new_empty_strings);
}
