#include "factory.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <vector>

#include "operations/begin.h"
#include "operations/serialize.h"

std::vector<std::shared_ptr<const Dictionary>> dictionaries;

std::vector<Character> characters;
std::vector<Conditional> conditionals;
std::vector<Function> functions;
std::vector<FunctionBuiltIn> built_in_functions;
std::vector<FunctionDictionary> dictionary_functions;
std::vector<FunctionList> list_functions;
std::vector<List> lists;
std::vector<EmptyList> empty_lists;
std::vector<LookupChild> child_lookups;
std::vector<FunctionApplication> function_applications;
std::vector<LookupSymbol> symbol_lookups;
std::vector<Name> names;
std::vector<Number> numbers;
std::vector<WhileStatement> while_statements;
std::vector<EndStatement> end_statements;
std::vector<Definition> definitions;
std::vector<String> strings;
std::vector<EmptyString> empty_strings;
std::vector<Boolean> booleans;
std::vector<Expression> expressions;

void clearMemory() {
    characters.clear();
    conditionals.clear();
    dictionaries.clear();
    functions.clear();
    built_in_functions.clear();
    dictionary_functions.clear();
    list_functions.clear();
    child_lookups.clear();
    function_applications.clear();
    symbol_lookups.clear();
    names.clear();
    numbers.clear();
    while_statements.clear();
    end_statements.clear();
    definitions.clear();
    strings.clear();
    booleans.clear();
    expressions.clear();
}

std::vector<size_t> whileIndices(const Statements& statements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    auto while_indices = std::vector<size_t>{};
    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        if (type == DEFINITION) {
            while_indices.push_back(1); // dummy
        }
        if (type == WHILE_STATEMENT) {
            while_positions.push_back(i);
            while_indices.push_back(1); // dummy
        }
        if (type == END_STATEMENT) {
            while_indices.push_back(while_positions.back());
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", begin(statements.front()));
    }
    return while_indices;
}

std::vector<size_t> endIndices(const Statements& statements) {
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    auto end_indices = std::vector<size_t>(statements.size());
    for (size_t i = statements.size() - 1; i < statements.size(); --i) {
        const auto type = statements[i].type;
        if (type == DEFINITION) {
            end_indices[i] = 0; // dummy
        }
        if (type == WHILE_STATEMENT) {
            end_indices[i] = end_positions.back();
            end_positions.pop_back();
        }
        if (type == END_STATEMENT) {
            end_indices[i] = 0; // dummy
            end_positions.push_back(i);
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", begin(statements.front()));
    }
    return end_indices;
}

std::vector<size_t> nameIndices(const Statements& statements) {
    // Forward pass to set name_index_:
    auto names = std::vector<std::string>{};
    auto name_indices = std::vector<size_t>{};
    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        if (type == DEFINITION) {
            const auto name = getName(getDefinition(statements[i]).name).value;
            const auto it = std::find(names.begin(), names.end(), name);
            name_indices.push_back(std::distance(names.begin(), it));
            if (it == names.end()) {
                names.push_back(name);
            }
        }
        if (type == WHILE_STATEMENT) {
            name_indices.push_back(0); // dummy
        }
        if (type == END_STATEMENT) {
            name_indices.push_back(0); // dummy
        }
    }
    return name_indices;
}

Statements setContext(const Statements& statements) {
    const auto while_indices = whileIndices(statements);
    const auto end_indices = endIndices(statements);
    const auto name_indices = nameIndices(statements);

    auto result = Statements{};

    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        if (type == DEFINITION) {
            const auto statement = getDefinition(statements[i]);
            result.push_back(makeDefinition({
                statement.range,
                statement.name,
                statement.expression,
                name_indices[i]
            }));
        } else if (type == WHILE_STATEMENT) {
            const auto statement = getWileStatement(statements[i]);
            result.push_back(makeWhileStatement({
                statement.range,
                statement.expression,
                end_indices[i],
            }));
        } else if (type == END_STATEMENT) {
            const auto statement = getEndStatement(statements[i]);
            result.push_back(makeEndStatement({
                statement.range, while_indices[i]
            }));
        } else {
            assert("Unexpected type");
        }
    }
    return result;
}

template<typename ElementType, typename ArrayType>
Expression makeMutableExpression(
    const ElementType* expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1});
    return expressions.back();
}

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    ElementType expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1});
    return expressions.back();
}

Expression makeNumber(Number expression) {
    return makeExpression(expression, NUMBER, numbers);
}

Expression makeCharacter(Character expression) {
    return makeExpression(expression, CHARACTER, characters);
}

Expression makeConditional(Conditional expression) {
    return makeExpression(expression, CONDITIONAL, conditionals);
}

Expression makeDictionary(const Dictionary* expression) {
    return makeMutableExpression(expression, DICTIONARY, dictionaries);
}

Expression makeFunction(Function expression) {
    return makeExpression(expression, FUNCTION, functions);
}

Expression makeFunctionBuiltIn(FunctionBuiltIn expression) {
    return makeExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

Expression makeFunctionDictionary(FunctionDictionary expression) {
    return makeExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

Expression makeFunctionList(FunctionList expression) {
    return makeExpression(expression, FUNCTION_LIST, list_functions);
}

Expression makeList(List expression) {
    return makeExpression(expression, LIST, lists);
}

Expression makeEmptyList(EmptyList expression) {
    return makeExpression(expression, EMPTY_LIST, empty_lists);
}

Expression makeLookupChild(LookupChild expression) {
    return makeExpression(expression, LOOKUP_CHILD, child_lookups);
}

Expression makeFunctionApplication(FunctionApplication expression) {
    return makeExpression(expression, FUNCTION_APPLICATION,
        function_applications);
}

Expression makeLookupSymbol(LookupSymbol expression) {
    return makeExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Expression makeName(Name expression) {
    return makeExpression(expression, NAME, names);
}

Expression makeDefinition(Definition expression) {
    return makeExpression(expression, DEFINITION, definitions);
}

Expression makeWhileStatement(WhileStatement expression) {
    return makeExpression(expression, WHILE_STATEMENT, while_statements);
}

Expression makeEndStatement(EndStatement expression) {
    return makeExpression(expression, END_STATEMENT, end_statements);
}

Expression makeString(String expression) {
    return makeExpression(expression, STRING, strings);
}

Expression makeEmptyString(EmptyString expression) {
    return makeExpression(expression, EMPTY_STRING, empty_strings);
}

Expression makeBoolean(Boolean expression) {
    return makeExpression(expression, BOOLEAN, booleans);
}

// FREE GETTERS

template<typename ArrayType>
typename ArrayType::value_type::element_type getMutableExpression(
    Expression expression,
    ExpressionType type,
    ArrayType& array
) {
    if (expression.type != type) {
        std::stringstream s;
        s << "getMutableExpression expected " << NAMES[type]
          << " got " << NAMES[expression.type];
        throw std::runtime_error{s.str()};
    }
    assert(expression.type == type);
    return *array.at(expression.index).get();
}

template<typename ArrayType>
typename ArrayType::value_type getExpression(
    Expression expression,
    ExpressionType type,
    const ArrayType& array
) {
    if (expression.type != type) {
        std::stringstream s;
        s << "getExpression expected " << NAMES[type]
            << " got " << NAMES[expression.type];
        throw std::runtime_error{s.str()};
    }
    assert(expression.type == type);
    return array.at(expression.index);
}

Definition getDefinition(Expression expression) {
    return getExpression(expression, DEFINITION, definitions);
}

WhileStatement getWileStatement(Expression expression) {
    return getExpression(expression, WHILE_STATEMENT, while_statements);
}

EndStatement getEndStatement(Expression expression) {
    return getExpression(expression, END_STATEMENT, end_statements);
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
    return getMutableExpression(expression, DICTIONARY, dictionaries);
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

EmptyList getEmptyList(Expression expression) {
    return getExpression(expression, EMPTY_LIST, empty_lists);
}

LookupChild getLookupChild(Expression expression) {
    return getExpression(expression, LOOKUP_CHILD, child_lookups);
}

FunctionApplication getFunctionApplication(Expression expression) {
    return getExpression(expression, FUNCTION_APPLICATION,
        function_applications);
}

LookupSymbol getLookupSymbol(Expression expression) {
    return getExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Name getName(Expression expression) {
    return getExpression(expression, NAME, names);
}

String getString(Expression expression) {
    return getExpression(expression, STRING, strings);
}

EmptyString getEmptyString(Expression expression) {
    return getExpression(expression, EMPTY_STRING, empty_strings);
}

Boolean getBoolean(Expression expression) {
    return getExpression(expression, BOOLEAN, booleans);
}

std::string getLog() {
    auto log = std::string{};
    for (const auto expression : expressions) {
        log += serialize(expression) + '\n';
    }
    return log;
}
