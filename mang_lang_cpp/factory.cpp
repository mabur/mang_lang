#include "factory.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <vector>

#include "operations/serialize.h"

std::vector<std::shared_ptr<const EvaluatedDictionary>> evaluated_dictionaries;

std::vector<Dictionary> dictionaries;
std::vector<Character> characters;
std::vector<Conditional> conditionals;
std::vector<IsExpression> is_expressions;
std::vector<Function> functions;
std::vector<FunctionBuiltIn> built_in_functions;
std::vector<FunctionDictionary> dictionary_functions;
std::vector<FunctionTuple> tuple_functions;
std::vector<Tuple> tuples;
std::vector<EvaluatedTuple> evaluated_tuples;
std::vector<Stack> stacks;
std::vector<EvaluatedStack> evaluated_stacks;
std::vector<EmptyStack> empty_stacks;
std::vector<LookupChild> child_lookups;
std::vector<FunctionApplication> function_applications;
std::vector<LookupSymbol> symbol_lookups;
std::vector<DynamicLookupSymbol> dynamic_symbol_lookups;
std::vector<Name> names;
std::vector<Label> labels;
std::vector<Number> numbers;
std::vector<WhileStatement> while_statements;
std::vector<EndStatement> end_statements;
std::vector<Definition> definitions;
std::vector<DynamicDefinition> dynamic_definitions;
std::vector<String> strings;
std::vector<EmptyString> empty_strings;
std::vector<Boolean> booleans;
std::vector<Expression> expressions;

namespace {

std::vector<size_t> whileIndices(const Statements& statements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    auto while_indices = std::vector<size_t>{};
    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        if (type == DEFINITION or type == DYNAMIC_DEFINITION) {
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
        if (type == DEFINITION or type == DYNAMIC_DEFINITION) {
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

} // namespace

Statements setContext(const Statements& statements) {
    const auto while_indices = whileIndices(statements);
    const auto end_indices = endIndices(statements);

    auto result = Statements{};

    for (size_t i = 0; i < statements.size(); ++i) {
        const auto type = statements[i].type;
        const auto code = statements[i].range;
        if (type == DEFINITION or type == DYNAMIC_DEFINITION) {
            result.push_back(statements[i]);
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

void clearMemory() {
    evaluated_dictionaries.clear();
    dictionaries.clear();
    characters.clear();
    conditionals.clear();
    is_expressions.clear();
    functions.clear();
    built_in_functions.clear();
    dictionary_functions.clear();
    tuple_functions.clear();
    stacks.clear();
    evaluated_stacks.clear();
    empty_stacks.clear();
    child_lookups.clear();
    function_applications.clear();
    symbol_lookups.clear();
    dynamic_symbol_lookups.clear();
    names.clear();
    labels.clear();
    numbers.clear();
    while_statements.clear();
    end_statements.clear();
    definitions.clear();
    dynamic_definitions.clear();
    strings.clear();
    empty_strings.clear();
    booleans.clear();
    expressions.clear();
}

std::string getLog() {
    auto log = std::string{};
    for (const auto expression : expressions) {
        log += serialize(expression) + '\n';
    }
    return log;
}

// MAKERS:

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

Expression makeFunctionTuple(CodeRange code, FunctionTuple expression) {
    return makeExpression(code, expression, FUNCTION_TUPLE, tuple_functions);
}

Expression makeTuple(CodeRange code, Tuple expression) {
    return makeExpression(code, expression, TUPLE, tuples);
}

Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression) {
    return makeExpression(code, expression, EVALUATED_TUPLE, evaluated_tuples);
}

Expression makeStack(CodeRange code, Stack expression) {
    return makeExpression(code, expression, STACK, stacks);
}

Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression) {
    return makeExpression(code, expression, EVALUATED_STACK, evaluated_stacks);
}

Expression makeEmptyStack(CodeRange code, EmptyStack expression) {
    return makeExpression(code, expression, EMPTY_STACK, empty_stacks);
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

Expression makeDynamicLookupSymbol(CodeRange code, DynamicLookupSymbol expression) {
    return makeExpression(code, expression, DYNAMIC_LOOKUP_SYMBOL, dynamic_symbol_lookups);
}

Expression makeName(CodeRange code, Name expression) {
    return makeExpression(code, expression, NAME, names);
}

Expression makeLabel(CodeRange code, Label expression) {
    return makeExpression(code, expression, LABEL, labels);
}

Expression makeDefinition(CodeRange code, Definition expression) {
    return makeExpression(code, expression, DEFINITION, definitions);
}

Expression makeDynamicDefinition(CodeRange code, DynamicDefinition expression) {
    return makeExpression(code, expression, DYNAMIC_DEFINITION, dynamic_definitions);
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

// GETTERS

Definition getDefinition(Expression expression) {
    return getExpression(expression, DEFINITION, definitions);
}

DynamicDefinition getDynamicDefinition(Expression expression) {
    return getExpression(expression, DYNAMIC_DEFINITION, dynamic_definitions);
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

Tuple getTuple(Expression expression) {
    return getExpression(expression, TUPLE, tuples);
}

EvaluatedTuple getEvaluatedTuple(Expression expression) {
    return getExpression(expression, EVALUATED_TUPLE, evaluated_tuples);
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

FunctionTuple getFunctionTuple(Expression expression) {
    return getExpression(expression, FUNCTION_TUPLE, tuple_functions);
}

Stack getStack(Expression expression) {
    return getExpression(expression, STACK, stacks);
}

EvaluatedStack getEvaluatedStack(Expression expression) {
    return getExpression(expression, EVALUATED_STACK, evaluated_stacks);
}

EmptyStack getEmptyStack(Expression expression) {
    return getExpression(expression, EMPTY_STACK, empty_stacks);
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

DynamicLookupSymbol getDynamicLookupSymbol(Expression expression) {
    return getExpression(expression, DYNAMIC_LOOKUP_SYMBOL, dynamic_symbol_lookups);
}

Name getName(Expression expression) {
    return getExpression(expression, NAME, names);
}

Label getLabel(Expression expression) {
    return getExpression(expression, LABEL, labels);
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
