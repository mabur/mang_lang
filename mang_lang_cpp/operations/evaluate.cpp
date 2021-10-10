#include "evaluate.h"

#include <cassert>

#include "../factory.h"
#include "boolean.h"
#include "serialize.h"
#include "../container.h"

Expression evaluateConditional(
    const Conditional& conditional, Expression environment, std::ostream& log
) {
    const auto result =
        boolean(evaluate(conditional.expression_if, environment, log)) ?
        evaluate(conditional.expression_then, environment, log) :
        evaluate(conditional.expression_else, environment, log);
    log << serialize(result) << std::endl;
    return result;
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
    const Dictionary& dictionary, Expression environment, std::ostream& log
) {
    const auto num_names = numNames(dictionary.statements);
    const auto result = new Dictionary{
        dictionary.range,
        environment,
        Statements(num_names, Expression{})
    };

    auto i = size_t{0};
    while (i < dictionary.statements.size()) {
        const auto statement = dictionary.statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            result->statements.at(definition.name_index_) = makeDefinition(
                new Definition{
                    definition.range,
                    definition.name,
                    evaluate(definition.expression, makeDictionary(result), log),
                    definition.name_index_
                }
            );
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWileStatement(statement);
            if (boolean(evaluate(while_statement.expression, makeDictionary(result), log))) {
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
    const auto wrapped_result = makeDictionary(result);
    log << serialize(wrapped_result) << std::endl;
    return wrapped_result;
}

Expression evaluateFunction(
    const Function& function, Expression environment, std::ostream& log
) {
    const auto result = makeFunction(new Function{
        function.range, environment, function.input_name, function.body
    });
    log << serialize(result) << std::endl;
    return result;
}

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment, std::ostream& log
) {
    const auto result = makeFunctionDictionary(
        new FunctionDictionary{
            function_dictionary.range,
            environment,
            function_dictionary.input_names,
            function_dictionary.body
        }
    );
    log << serialize(result) << std::endl;
    return result;
}

Expression evaluateFunctionList(
    const FunctionList& function_list, Expression environment, std::ostream& log
) {
    const auto result = makeFunctionList(new FunctionList{
        function_list.range, environment, function_list.input_names, function_list.body
    });
    log << serialize(result) << std::endl;
    return result;
}

Expression evaluateList(
    Expression list, Expression environment, std::ostream& log
) {
    const auto op = [&](Expression rest, Expression first) -> Expression {
        const auto evaluated_first = evaluate(first, environment, log);
        return new_list::prepend(rest, evaluated_first);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyList(new EmptyList{});
    const auto output = new_list::leftFold(init, list, op);
    auto result = new_list::reverse(code, output);

    log << serialize(result) << std::endl;
    return result;
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
        throw std::runtime_error("Cannot find symbol " + name);
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
    throw std::runtime_error("Dictionary does not contain symbol " + name);
}

Expression lookupChildInString(const String& string, const std::string& name) {
    if (name == "first") {
        return string.first;
    }
    if (name == "rest") {
        return string.rest;
    }
    throw std::runtime_error("String does not contain symbol " + name);
}

Expression lookupChildInList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.first;
    }
    if (name == "rest") {
        return list.rest;
    }
    throw std::runtime_error("List does not contain symbol " + name);
}

Expression lookupChild(Expression expression, const std::string& name) {
    switch(expression.type) {
        case DICTIONARY: return lookupChildInDictionary(getDictionary(expression), name);
        case LIST: return lookupChildInList(getList(expression), name);
        case STRING: return lookupChildInString(getString(expression), name);
        default: throw std::runtime_error{"Cannot getLookupChild expression of type " + std::to_string(expression.type)};
    }
}

Expression evaluateLookupChild(
    const LookupChild& lookup_child, Expression environment, std::ostream& log
) {
    const auto child = evaluate(lookup_child.child, environment, log);
    const auto result = lookupChild(child, getName(lookup_child.name).value);
    log << serialize(result) << std::endl;
    return result;
}

Expression applyFunction(
    const Function& function, Expression input, std::ostream& log
) {

    const auto statements = Statements{
        makeDefinition(new Definition{function.range, function.input_name, input, 0})
    };
    const auto middle = makeDictionary(
        new Dictionary{CodeRange{}, function.environment, statements}
    );
    return evaluate(function.body, middle, log);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input, std::ostream&
) {
    return function_built_in.function(input);
}

Expression applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression input, std::ostream& log
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary.body, input, log);
}

Expression applyFunctionList(const FunctionList& function_list, Expression input, std::ostream& log
) {
    auto statements = Statements{};
    auto expression = input;
    for (size_t i = 0; i < function_list.input_names.size(); ++i) {
        const auto list = getList(expression);
        statements.push_back(
            makeDefinition(
                new Definition{
                    function_list.range,
                    function_list.input_names[i],
                    list.first,
                    i
                }
            )
        );
        expression = list.rest;
    }
    const auto middle = makeDictionary(
        new Dictionary{function_list.range, function_list.environment, statements}
    );
    return evaluate(function_list.body, middle, log);
}

Expression apply(Expression expression, Expression input, std::ostream& log) {
    switch (expression.type) {
        case FUNCTION: return applyFunction(getFunction(expression), input, log);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(expression), input, log);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(getFunctionDictionary(expression), input, log);
        case FUNCTION_LIST: return applyFunctionList(getFunctionList(expression), input, log);
        default: throw std::runtime_error{"Expected getFunction"};
    }
}

Expression evaluateFunctionApplication(
    const FunctionApplication& function_application, Expression environment, std::ostream& log
) {
    const auto function = lookupDictionary(environment, getName(function_application.name).value);
    const auto evaluated_child = evaluate(function_application.child, environment, log);
    const auto result = apply(function, evaluated_child, log);
    log << serialize(result) << std::endl;
    return result;
}

Expression evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, Expression environment, std::ostream& log
) {
    const auto result = lookupDictionary(environment, getName(lookup_symbol.name).value);
    log << serialize(result) << std::endl;
    return result;
}

Expression evaluateAtom(
    Expression atom, std::ostream& log
) {
    log << serialize(atom) << std::endl;
    return atom;
}

Expression evaluate(
    Expression expression,
    Expression environment,
    std::ostream& log
) {
    switch (expression.type) {
        case NUMBER: return evaluateAtom(expression, log);
        case CHARACTER: return evaluateAtom(expression, log);
        case BOOLEAN: return evaluateAtom(expression, log);
        case EMPTY_STRING: return evaluateAtom(expression, log);
        case STRING: return evaluateAtom(expression, log);
        case EMPTY_LIST: return evaluateAtom(expression, log);

        case FUNCTION: return evaluateFunction(getFunction(expression), environment, log);
        case FUNCTION_LIST: return evaluateFunctionList(getFunctionList(expression), environment, log);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment, log);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment, log);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment, log);
        case LIST: return evaluateList(expression, environment, log);
        case LOOKUP_CHILD: return evaluateLookupChild(getLookupChild(expression), environment, log);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment, log);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(getLookupSymbol(expression), environment, log);
        default: throw std::runtime_error{"Did not recognize expression to evaluate"};
    }
}
