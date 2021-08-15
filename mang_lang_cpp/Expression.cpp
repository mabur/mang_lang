#include "Expression.h"

#include <cassert>
#include <vector>

#include "operations/begin.h"

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
            const auto name = elements[i].namedElement().name->value;
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
            const auto element = elements[i].namedElement();
            result.push_back(makeNamedElement(new NamedElement{
                element.range,
                element.name,
                element.expression,
                name_indices[i]
            }));
        } else if (type == WHILE_ELEMENT) {
            const auto element = elements[i].whileElement();
            result.push_back(makeWhileElement(new WhileElement{
                element.range,
                element.expression,
                end_indices[i],
            }));
        } else if (type == END_ELEMENT) {
            const auto element = elements[i].endElement();
            result.push_back(makeEndElement(new EndElement{
                element.range, while_indices[i]
            }));
        } else {
            assert("Unexpected type");
        }
    }
    return result;
}

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

ExpressionPointer makeNumber(const Number* expression) {
    numbers.emplace_back(expression);
    return ExpressionPointer{NUMBER, numbers.size() - 1};
}

ExpressionPointer makeCharacter(const Character* expression) {
    characters.emplace_back(expression);
    return ExpressionPointer{CHARACTER, characters.size() - 1};
}

ExpressionPointer makeConditional(const Conditional* expression) {
    conditionals.emplace_back(expression);
    return ExpressionPointer{CONDITIONAL, conditionals.size() - 1};
}

ExpressionPointer makeDictionary(const Dictionary* expression) {
    dictionaries.emplace_back(expression);
    return ExpressionPointer{DICTIONARY, dictionaries.size() - 1};
}

ExpressionPointer makeFunction(const Function* expression) {
    functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION, functions.size() - 1};
}

ExpressionPointer makeFunctionBuiltIn(const FunctionBuiltIn* expression) {
    built_in_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_BUILT_IN, built_in_functions.size() - 1};
}

ExpressionPointer makeFunctionDictionary(const FunctionDictionary* expression) {
    dictionary_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_DICTIONARY, dictionary_functions.size() - 1};
}

ExpressionPointer makeFunctionList(const FunctionList* expression) {
    list_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_LIST, list_functions.size() - 1};
}

ExpressionPointer makeList(const List* expression) {
    lists.emplace_back(expression);
    return ExpressionPointer{LIST, lists.size() - 1};
}

ExpressionPointer makeLookupChild(const LookupChild* expression) {
    child_lookups.emplace_back(expression);
    return ExpressionPointer{LOOKUP_CHILD, child_lookups.size() - 1};
}

ExpressionPointer makeFunctionApplication(const FunctionApplication* expression) {
    function_applications.emplace_back(expression);
    return ExpressionPointer{FUNCTION_APPLICATION, function_applications.size() - 1};
}

ExpressionPointer makeLookupSymbol(const LookupSymbol* expression) {
    symbol_lookups.emplace_back(expression);
    return ExpressionPointer{LOOKUP_SYMBOL, symbol_lookups.size() - 1};
}

ExpressionPointer makeName(const Name* expression) {
    names.emplace_back(expression);
    return ExpressionPointer{NAME, names.size() - 1};
}

ExpressionPointer makeString(const String* expression) {
    strings.emplace_back(expression);
    return ExpressionPointer{STRING, strings.size() - 1};
}

ExpressionPointer makeNamedElement(const NamedElement* expression) {
    named_elements.emplace_back(expression);
    return ExpressionPointer{NAMED_ELEMENT, named_elements.size() - 1};
}

ExpressionPointer makeWhileElement(const WhileElement* expression) {
    while_elements.emplace_back(expression);
    return ExpressionPointer{WHILE_ELEMENT, while_elements.size() - 1};
}

ExpressionPointer makeEndElement(const EndElement* expression) {
    end_elements.emplace_back(expression);
    return ExpressionPointer{END_ELEMENT, end_elements.size() - 1};
}

NamedElement ExpressionPointer::namedElement() const {
    assert(type == NAMED_ELEMENT);
    return *named_elements.at(index).get();
}

WhileElement ExpressionPointer::whileElement() const {
    assert(type == WHILE_ELEMENT);
    return *while_elements.at(index).get();
}

EndElement ExpressionPointer::endElement() const {
    assert(type == END_ELEMENT);
    return *end_elements.at(index).get();
}

Number ExpressionPointer::number() const {
    assert(type == NUMBER);
    return *numbers.at(index).get();
}

Character ExpressionPointer::character() const {
    assert(type == CHARACTER);
    return *characters.at(index).get();
}

Conditional ExpressionPointer::conditional() const {
    assert(type == CONDITIONAL);
    return *conditionals.at(index).get();
}

Dictionary ExpressionPointer::dictionary() const {
    assert(type == DICTIONARY);
    return *dictionaries.at(index).get();
}

Function ExpressionPointer::function() const {
    assert(type == FUNCTION);
    return *functions.at(index).get();
}

FunctionBuiltIn ExpressionPointer::functionBuiltIn() const {
    assert(type == FUNCTION_BUILT_IN);
    return *built_in_functions.at(index).get();
}

FunctionDictionary ExpressionPointer::functionDictionary() const {
    assert(type == FUNCTION_DICTIONARY);
    return *dictionary_functions.at(index).get();
}

FunctionList ExpressionPointer::functionList() const {
    assert(type == FUNCTION_LIST);
    return *list_functions.at(index).get();
}

List ExpressionPointer::list() const {
    assert(type == LIST);
    return *lists.at(index).get();
}

LookupChild ExpressionPointer::lookupChild() const {
    assert(type == LOOKUP_CHILD);
    return *child_lookups.at(index).get();
}

FunctionApplication ExpressionPointer::functionApplication() const {
    assert(type == FUNCTION_APPLICATION);
    return *function_applications.at(index).get();
}

LookupSymbol ExpressionPointer::lookupSymbol() const {
    assert(type == LOOKUP_SYMBOL);
    return *symbol_lookups.at(index).get();
}

Name ExpressionPointer::name() const {
    assert(type == NAME);
    return *names.at(index).get();
}

String ExpressionPointer::string() const {
    assert(type == STRING);
    return *strings.at(index).get();
}

// FREE GETTERS

NamedElement namedElement(ExpressionPointer expression) {
    assert(expression.type == NAMED_ELEMENT);
    return *named_elements.at(expression.index).get();
}

WhileElement whileElement(ExpressionPointer expression) {
    assert(expression.type == WHILE_ELEMENT);
    return *while_elements.at(expression.index).get();
}

EndElement endElement(ExpressionPointer expression) {
    assert(expression.type == END_ELEMENT);
    return *end_elements.at(expression.index).get();
}

Number number2(ExpressionPointer expression) {
    assert(expression.type == NUMBER);
    return *numbers.at(expression.index).get();
}

Character character(ExpressionPointer expression) {
    assert(expression.type == CHARACTER);
    return *characters.at(expression.index).get();
}

Conditional conditional(ExpressionPointer expression) {
    assert(expression.type == CONDITIONAL);
    return *conditionals.at(expression.index).get();
}

Dictionary dictionary(ExpressionPointer expression) {
    assert(expression.type == DICTIONARY);
    return *dictionaries.at(expression.index).get();
}

Function function(ExpressionPointer expression) {
    assert(expression.type == FUNCTION);
    return *functions.at(expression.index).get();
}

FunctionBuiltIn functionBuiltIn(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_BUILT_IN);
    return *built_in_functions.at(expression.index).get();
}

FunctionDictionary functionDictionary(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_DICTIONARY);
    return *dictionary_functions.at(expression.index).get();
}

FunctionList functionList(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_LIST);
    return *list_functions.at(expression.index).get();
}

List list2(ExpressionPointer expression) {
    assert(expression.type == LIST);
    return *lists.at(expression.index).get();
}

LookupChild lookupChild(ExpressionPointer expression) {
    assert(expression.type == LOOKUP_CHILD);
    return *child_lookups.at(expression.index).get();
}

FunctionApplication functionApplication(ExpressionPointer expression) {
    assert(expression.type == FUNCTION_APPLICATION);
    return *function_applications.at(expression.index).get();
}

LookupSymbol lookupSymbol(ExpressionPointer expression) {
    assert(expression.type == LOOKUP_SYMBOL);
    return *symbol_lookups.at(expression.index).get();
}

Name name(ExpressionPointer expression) {
    assert(expression.type == NAME);
    return *names.at(expression.index).get();
}

String string(ExpressionPointer expression) {
    assert(expression.type == STRING);
    return *strings.at(expression.index).get();
}
