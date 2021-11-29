#include "evaluate.h"

#include <cassert>

#include "../factory.h"
#include "boolean.h"
#include "serialize.h"
#include "../container.h"

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    return
        boolean(evaluate(conditional.expression_if, environment)) ?
        evaluate(conditional.expression_then, environment) :
        evaluate(conditional.expression_else, environment);
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
    const auto result = new Dictionary{
        dictionary.range,
        environment,
        Statements(num_names, Expression{})
    };
    const auto dictionary_result = makeDictionary(result);
    auto i = size_t{0};
    while (i < dictionary.statements.size()) {
        const auto statement = dictionary.statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            result->statements.at(definition.name_index_) = makeDefinition({
                definition.range,
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
    return makeFunction({
        function.range, environment, function.input_name, function.body
    });
}

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
) {
    return makeFunctionDictionary({
        function_dictionary.range,
        environment,
        function_dictionary.input_names,
        function_dictionary.body
    });
}

Expression evaluateFunctionList(
    const FunctionList& function_list, Expression environment
) {
    return makeFunctionList({
        function_list.range, environment, function_list.input_names, function_list.body
    });
}

Expression evaluateList(
    Expression list, Expression environment
) {
    const auto op = [&](Expression rest, Expression first) -> Expression {
        const auto evaluated_first = evaluate(first, environment);
        return new_list::prepend(rest, evaluated_first);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyList({});
    const auto output = new_list::leftFold(init, list, op);
    return new_list::reverse(code, output);
}

Expression lookupCurrentDictionary(const Dictionary& dictionary, const std::string& name) {
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
    if (expression.type != DICTIONARY) {
        throw MissingSymbol(name, "environment");
    }
    const auto d = getDictionary(expression);
    const auto result = lookupCurrentDictionary(d, name);
    if (result.type != EMPTY) {
        return result;
    }
    return lookupDictionary(d.environment, name);
}

Expression lookupChildInDictionary(const Dictionary& dictionary, const std::string& name) {
    const auto result = lookupCurrentDictionary(dictionary, name);
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

Expression lookupChildInList(const List& list, const std::string& name) {
    if (name == "top") {
        return list.first;
    }
    if (name == "rest") {
        return list.rest;
    }
    throw MissingSymbol(name, "list");
}

Expression evaluateLookupChild(
    const LookupChild& lookup_child, Expression environment
) {
    const auto child = evaluate(lookup_child.child, environment);
    const auto name = getName(lookup_child.name).value;
    switch(child.type) {
        case DICTIONARY: return lookupChildInDictionary(getDictionary(child), name);
        case LIST: return lookupChildInList(getList(child), name);
        case STRING: return lookupChildInString(getString(child), name);
        default: throw UnexpectedExpression(child.type, "evaluateLookupChild");
    }
}

Expression applyFunction(const Function& function, Expression input) {

    const auto statements = Statements{
        makeDefinition({function.range, function.input_name, input, 0})
    };
    const auto middle = makeDictionary(
        new Dictionary{CodeRange{}, function.environment, statements}
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
        const auto list = getList(expression);
        statements.push_back(makeDefinition({
            function_list.range,
            function_list.input_names[i],
            list.first,
            i
        }));
        expression = list.rest;
    }
    const auto middle = makeDictionary(
        new Dictionary{function_list.range, function_list.environment, statements}
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

        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_LIST: return evaluateFunctionList(getFunctionList(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case LIST: return evaluateList(expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(getLookupSymbol(expression), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
