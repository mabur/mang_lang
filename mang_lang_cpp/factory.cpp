#include "factory.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <vector>

#include "operations/serialize.h"

std::vector<Dictionary> dictionaries;
std::vector<std::shared_ptr<const EvaluatedDictionary>> evaluated_dictionaries;

std::vector<Character> characters;
std::vector<Conditional> conditionals;
std::vector<IsExpression> is_expressions;
std::vector<Function> functions;
std::vector<FunctionBuiltIn> built_in_functions;
std::vector<FunctionDictionary> dictionary_functions;
std::vector<FunctionList> list_functions;
std::vector<List> lists;
std::vector<EvaluatedList> evaluated_lists;
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
        throw ParseException("More while than end", statements.front().range.first);
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
        throw ParseException("Fewer while than end", statements.front().range.first);
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

    auto result = Statements{};

    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        const auto code = statements[i].range;
        if (type == DEFINITION) {
            const auto statement = getDefinition(statements[i]);
            result.push_back(makeDefinition(code, {
                statement.name,
                statement.expression,
            }));
        } else if (type == WHILE_STATEMENT) {
            const auto statement = getWileStatement(statements[i]);
            result.push_back(makeWhileStatement(code, {
                statement.expression,
                end_indices[i],
            }));
        } else if (type == END_STATEMENT) {
            result.push_back(makeEndStatement(code, {while_indices[i]}));
        } else {
            assert("Unexpected type");
        }
    }
    return result;
}

template<typename ElementType, typename ArrayType>
Expression makeMutableExpression(
    CodeRange code,
    const ElementType* expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1, code});
    return expressions.back();
}

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    CodeRange code,
    ElementType expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1, code});
    return expressions.back();
}

Expression makeNumber(CodeRange code, Number expression) {
    return makeExpression(code, expression, NUMBER, numbers);
}

Expression makeCharacter(CodeRange code, Character expression) {
    return makeExpression(code, expression, CHARACTER, characters);
}

Expression makeConditional(CodeRange code, Conditional expression) {
    return makeExpression(code, expression, CONDITIONAL, conditionals);
}

Expression makeIs(CodeRange code, IsExpression expression) {
    return makeExpression(code, expression, IS, is_expressions);
}

Expression makeDictionary(CodeRange code, const Dictionary expression) {
    return makeExpression(code, expression, DICTIONARY, dictionaries);
}

Expression makeEvaluatedDictionary(CodeRange code, const EvaluatedDictionary* expression) {
    return makeMutableExpression(code, expression, EVALUATED_DICTIONARY, evaluated_dictionaries);
}

Expression makeFunction(CodeRange code, Function expression) {
    return makeExpression(code, expression, FUNCTION, functions);
}

Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression) {
    return makeExpression(code, expression, FUNCTION_BUILT_IN, built_in_functions);
}

Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression) {
    return makeExpression(code, expression, FUNCTION_DICTIONARY, dictionary_functions);
}

Expression makeFunctionList(CodeRange code, FunctionList expression) {
    return makeExpression(code, expression, FUNCTION_LIST, list_functions);
}

Expression makeList(CodeRange code, List expression) {
    return makeExpression(code, expression, LIST, lists);
}

Expression makeEvaluatedList(CodeRange code, EvaluatedList expression) {
    return makeExpression(code, expression, EVALUATED_LIST, evaluated_lists);
}

Expression makeEmptyList(CodeRange code, EmptyList expression) {
    return makeExpression(code, expression, EMPTY_LIST, empty_lists);
}

Expression makeLookupChild(CodeRange code, LookupChild expression) {
    return makeExpression(code, expression, LOOKUP_CHILD, child_lookups);
}

Expression makeFunctionApplication(CodeRange code, FunctionApplication expression) {
    return makeExpression(code, expression, FUNCTION_APPLICATION, function_applications);
}

Expression makeLookupSymbol(CodeRange code, LookupSymbol expression) {
    return makeExpression(code, expression, LOOKUP_SYMBOL, symbol_lookups);
}

Expression makeName(CodeRange code, Name expression) {
    return makeExpression(code, expression, NAME, names);
}

Expression makeDefinition(CodeRange code, Definition expression) {
    return makeExpression(code, expression, DEFINITION, definitions);
}

Expression makeWhileStatement(CodeRange code, WhileStatement expression) {
    return makeExpression(code, expression, WHILE_STATEMENT, while_statements);
}

Expression makeEndStatement(CodeRange code, EndStatement expression) {
    return makeExpression(code, expression, END_STATEMENT, end_statements);
}

Expression makeString(CodeRange code, String expression) {
    return makeExpression(code, expression, STRING, strings);
}

Expression makeEmptyString(CodeRange code, EmptyString expression) {
    return makeExpression(code, expression, EMPTY_STRING, empty_strings);
}

Expression makeBoolean(CodeRange code, Boolean expression) {
    return makeExpression(code, expression, BOOLEAN, booleans);
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

IsExpression getIs(Expression expression) {
    return getExpression(expression, IS, is_expressions);
}

Dictionary getDictionary(Expression expression) {
    return getExpression(expression, DICTIONARY, dictionaries);
}

EvaluatedDictionary getEvaluatedDictionary(Expression expression) {
    return getMutableExpression(expression, EVALUATED_DICTIONARY, evaluated_dictionaries);
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

EvaluatedList getEvaluatedList(Expression expression) {
    return getExpression(expression, EVALUATED_LIST, evaluated_lists);
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
