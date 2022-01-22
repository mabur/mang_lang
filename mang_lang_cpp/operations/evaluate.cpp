#include "evaluate.h"

#include <cassert>

#include "../factory.h"
#include "boolean.h"
#include "serialize.h"
#include "../container.h"

bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left).value == getNumber(right).value;
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left).value == getCharacter(right).value;
    }
    if (left_type == BOOLEAN && right_type == BOOLEAN) {
        return getBoolean(left).value == getBoolean(right).value;
    }
    if (left_type == EMPTY_LIST && right_type == EMPTY_LIST) {
        return true;
    }
    if (left_type == EVALUATED_LIST && right_type == EVALUATED_LIST) {
        return allOfPairs(left, right, isEqual, EMPTY_LIST, getEvaluatedList);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return allOfPairs(left, right, isEqual, EMPTY_STRING, getString);
    }
    return false;
}

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    return
        boolean(evaluate(conditional.expression_if, environment)) ?
        evaluate(conditional.expression_then, environment) :
        evaluate(conditional.expression_else, environment);
}

Expression evaluateIs(
    const IsExpression& is_expression, Expression environment
) {
    const auto value = evaluate(is_expression.input, environment);
    for (const auto alternative : is_expression.alternatives) {
        const auto left_value = evaluate(alternative.left, environment);
        if (isEqual(value, left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(is_expression.expression_else, environment);
}

size_t numNames(const Statements& statements) {
    if (statements.empty()) {
        return 0;
    }
    auto max_index = size_t{0};
    for (const auto& statement : statements) {
        if (statement.type == DEFINITION) {
            max_index = std::max(max_index, getDefinition(statement).name_index_);
        }
    }
    return max_index + 1;
}

Expression evaluateDictionary(
    const Dictionary& dictionary, Expression environment
) {
    const auto num_names = numNames(dictionary.statements);
    const auto result = new EvaluatedDictionary{
        environment,
        Statements(num_names, Expression{})
    };
    const auto dictionary_result = makeEvaluatedDictionary(CodeRange{}, result);
    auto i = size_t{0};
    while (i < dictionary.statements.size()) {
        const auto statement = dictionary.statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            result->statements.at(definition.name_index_) = makeDefinition(
                statement.range,
                {
                    definition.name,
                    evaluate(definition.expression, dictionary_result),
                    definition.name_index_
            });
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWileStatement(statement);
            if (boolean(evaluate(while_statement.expression, dictionary_result))) {
                i += 1;
            } else {
                i = while_statement.end_index_ + 1;
            }
        }
        else if (type == END_STATEMENT) {
            const auto end_statement = getEndStatement(statement);
            i = end_statement.while_index_;
        }
    }
    return dictionary_result;
}

Expression evaluateFunction(const Function& function, Expression environment) {
    return makeFunction(CodeRange{}, {
        environment, function.input_name, function.body
    });
}

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
) {
    return makeFunctionDictionary(CodeRange{}, {
        environment,
        function_dictionary.input_names,
        function_dictionary.body
    });
}

Expression evaluateFunctionList(
    const FunctionList& function_list, Expression environment
) {
    return makeFunctionList(CodeRange{}, {
        environment, function_list.input_names, function_list.body
    });
}

Expression evaluateList(
    Expression list, Expression environment
) {
    const auto op = [&](Expression rest, Expression first) -> Expression {
        const auto evaluated_first = evaluate(first, environment);
        return putEvaluatedList(rest, evaluated_first);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyList(code, {});
    const auto output = leftFold(init, list, op, EMPTY_LIST, getList);
    return reverseEvaluatedList(code, output);
}

Expression lookupCurrentEvaluatedDictionary(const EvaluatedDictionary& dictionary, const std::string& name) {
    for (const auto& statement : dictionary.statements) {
        if (statement.type == DEFINITION) {
            const auto definition = getDefinition(statement);
            if (getName(definition.name).value == name) {
                return definition.expression;
            }
        }
    }
    return Expression{};
}

Expression lookupDictionary(Expression expression, const std::string& name) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(name, "environment");
    }
    const auto d = getEvaluatedDictionary(expression);
    const auto result = lookupCurrentEvaluatedDictionary(d, name);
    if (result.type != EMPTY) {
        return result;
    }
    return lookupDictionary(d.environment, name);
}

Expression lookupChildInEvaluatedDictionary(const EvaluatedDictionary& dictionary, const std::string& name) {
    const auto result = lookupCurrentEvaluatedDictionary(dictionary, name);
    if (result.type != EMPTY) {
        return result;
    }
    throw MissingSymbol(name, "dictionary");
}

Expression lookupChildInString(const String& string, const std::string& name) {
    if (name == "top") {
        return string.first;
    }
    if (name == "rest") {
        return string.rest;
    }
    throw MissingSymbol(name, "string");
}

Expression lookupChildInEvaluatedList(const EvaluatedList& list, const std::string& name) {
    if (name == "top") {
        return list.first;
    }
    if (name == "rest") {
        return list.rest;
    }
    throw MissingSymbol(name, "evaluated_list");
}

Expression evaluateLookupChild(
    const LookupChild& lookup_child, Expression environment
) {
    const auto child = evaluate(lookup_child.child, environment);
    const auto name = getName(lookup_child.name).value;
    switch(child.type) {
        case EVALUATED_DICTIONARY: return lookupChildInEvaluatedDictionary(getEvaluatedDictionary(child), name);
        case EVALUATED_LIST: return lookupChildInEvaluatedList(getEvaluatedList(child), name);
        case STRING: return lookupChildInString(getString(child), name);
        default: throw UnexpectedExpression(child.type, "evaluateLookupChild");
    }
}

Expression applyFunction(const Function& function, Expression input) {

    const auto statements = Statements{
        makeDefinition(CodeRange{}, {function.input_name, input, 0})
    };
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        new EvaluatedDictionary{function.environment, statements}
    );
    return evaluate(function.body, middle);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
) {
    return function_built_in.function(input);
}

Expression applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression input
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary.body, input);
}

Expression applyFunctionList(const FunctionList& function_list, Expression input
) {
    auto statements = Statements{};
    auto expression = input;
    for (size_t i = 0; i < function_list.input_names.size(); ++i) {
        const auto list = getEvaluatedList(expression);
        statements.push_back(makeDefinition(CodeRange{}, {
            function_list.input_names[i],
            list.first,
            i
        }));
        expression = list.rest;
    }
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        new EvaluatedDictionary{function_list.environment, statements}
    );
    return evaluate(function_list.body, middle);
}

Expression evaluateFunctionApplication(
    const FunctionApplication& function_application, Expression environment
) {
    const auto function = lookupDictionary(environment, getName(function_application.name).value);
    const auto input = evaluate(function_application.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(getFunctionDictionary(function), input);
        case FUNCTION_LIST: return applyFunctionList(getFunctionList(function), input);
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplication");
    }
}

Expression evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, Expression environment
) {
    return lookupDictionary(environment, getName(lookup_symbol.name).value);
}

Expression evaluate(Expression expression, Expression environment) {
    switch (expression.type) {
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case BOOLEAN: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_LIST: return expression;
        case EVALUATED_LIST: return expression;
        case EVALUATED_DICTIONARY: return expression;

        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_LIST: return evaluateFunctionList(getFunctionList(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case IS: return evaluateIs(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case LIST: return evaluateList(expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(getLookupSymbol(expression), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
