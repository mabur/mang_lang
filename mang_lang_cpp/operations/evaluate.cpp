#include "evaluate.h"

#include <cassert>

#include "boolean.h"
#include "serialize.h"

ExpressionPointer evaluateConditional(
    const Conditional& conditional, ExpressionPointer environment, std::ostream& log
) {
    auto result = boolean(evaluate(conditional.expression_if, environment, log)) ?
        evaluate(conditional.expression_then, environment, log) :
        evaluate(conditional.expression_else, environment, log);
    log << serialize(result) << std::endl;
    return result;
}

size_t numNames(const DictionaryElements& elements) {
    if (elements.empty()) {
        return 0;
    }
    auto max_index = size_t{0};
    for (const auto& element : elements) {
        if (element.type == NAMED_ELEMENT) {
            max_index = std::max(max_index, element.namedElement().name_index_);
        }
    }
    return max_index + 1;
}

ExpressionPointer evaluateDictionary(
    const Dictionary& dictionary, ExpressionPointer environment, std::ostream& log
) {
    const auto num_names = numNames(dictionary.elements);
    const auto result = new Dictionary{
        dictionary.range,
        environment,
        DictionaryElements(num_names, ExpressionPointer{})
    };
    const auto wrapped_result = makeDictionary(result);
    auto i = size_t{0};
    while (i < dictionary.elements.size()) {
        const auto type = dictionary.elements[i].type;
        if (type == NAMED_ELEMENT) {
            const auto element = dictionary.elements[i].namedElement();
            result->elements.at(element.name_index_) = makeNamedElement(
                new NamedElement{
                    element.range,
                    element.name,
                    evaluate(element.expression, wrapped_result, log),
                    element.name_index_
                }
            );
            i += 1;
        }
        else if (type == WHILE_ELEMENT) {
            const auto element = dictionary.elements[i].whileElement();
            if (boolean(evaluate(element.expression, wrapped_result, log))) {
                i += 1;
            } else {
                i = element.end_index_ + 1;
            }
        }
        else if (type == END_ELEMENT) {
            const auto element = dictionary.elements[i].endElement();
            i = element.while_index_;
        }
    }
    log << serialize(wrapped_result) << std::endl;
    return wrapped_result;
}

ExpressionPointer evaluateFunction(
    const Function& function, ExpressionPointer environment, std::ostream& log
) {
    auto result = makeFunction(new Function{
        function.range, environment, function.input_name, function.body
    });
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, ExpressionPointer environment, std::ostream& log
) {
    auto result = makeFunctionDictionary(
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

ExpressionPointer evaluateFunctionList(
    const FunctionList& function_list, ExpressionPointer environment, std::ostream& log
) {
    auto result = makeFunctionList(new FunctionList{
        function_list.range, environment, function_list.input_names, function_list.body
    });
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateList(
    const List& list, ExpressionPointer environment, std::ostream& log
) {
    const auto operation = [&](ExpressionPointer expression) -> ExpressionPointer {
        return evaluate(expression, environment, log);
    };
    auto evaluated_elements = map(list.elements, operation);
    auto result = makeList(new List{list.range, std::move(evaluated_elements)});
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer lookupDictionary(const ExpressionPointer& expression, const std::string& name) {
    if (expression.type != DICTIONARY) {
        throw std::runtime_error("Cannot find symbol " + name);
    }
    const auto dictionary = expression.dictionary();
    for (const auto& element : dictionary.elements) {
        if (element.type == NAMED_ELEMENT) {
            const auto dictionary_element = element.namedElement();
            if (dictionary_element.name->value == name) {
                return dictionary_element.expression;
            }
        }
    }
    return lookupDictionary(dictionary.environment, name);
}

ExpressionPointer lookupChildInDictionary(const Dictionary& dictionary, const std::string& name) {
    for (const auto& element : dictionary.elements) {
        if (element.type == NAMED_ELEMENT) {
            const auto dictionary_element = element.namedElement();
            if (dictionary_element.name->value == name) {
                return dictionary_element.expression;
            }
        }
    }
    throw std::runtime_error("Dictionary does not contain symbol " + name);
}

ExpressionPointer lookupChildInList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.elements->first;
    }
    if (name == "rest") {
        return makeList(new List{list.range, list.elements->rest});
    }
    throw std::runtime_error("List does not contain symbol " + name);
}

ExpressionPointer lookupChildInString(const String& string, const std::string& name) {
    if (name == "first") {
        return string.elements->first;
    }
    if (name == "rest") {
        return makeString(new String{string.range, string.elements->rest});
    }
    throw std::runtime_error("String does not contain symbol " + name);
}

ExpressionPointer lookupChild(ExpressionPointer expression, const std::string& name) {
    switch(expression.type) {
        case DICTIONARY: return lookupChildInDictionary(expression.dictionary(), name);
        case LIST: return lookupChildInList(expression.list(), name);
        case STRING: return lookupChildInString(expression.string(), name);
        default: throw std::runtime_error{"Cannot lookupChild expression of type " + std::to_string(expression.type)};
    }
}

ExpressionPointer evaluateLookupChild(
    const LookupChild& lookup_child, ExpressionPointer environment, std::ostream& log
) {
    auto result = lookupChild(
        evaluate(lookup_child.child, environment, log),
        lookup_child.name->value
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer applyFunction(
    const Function& function, ExpressionPointer input, std::ostream& log
) {

    const auto elements = DictionaryElements{
        makeNamedElement(
            new NamedElement{
                function.range,
                function.input_name,
                input,
                0
            }
        )
    };
    const auto middle = makeDictionary(
        new Dictionary{CodeRange{}, function.environment, elements}
    );
    return evaluate(function.body, middle, log);
}

ExpressionPointer applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, ExpressionPointer input, std::ostream&
) {
    return function_built_in.function(input);
}

ExpressionPointer applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary.body, input, log);
}

ExpressionPointer applyFunctionList(const FunctionList& function_list, ExpressionPointer input, std::ostream& log
) {
    auto elements = DictionaryElements{};
    auto i = size_t{0};
    for (auto list = input.list().elements; list; list = list->rest, ++i) {
        elements.push_back(
            makeNamedElement(
                new NamedElement{
                    function_list.range,
                    function_list.input_names[i],
                    list->first,
                    i
                }
            )
        );
    }
    const auto middle = makeDictionary(
        new Dictionary{
            function_list.range, function_list.environment, elements
        }
    );
    return evaluate(function_list.body, middle, log);
}

ExpressionPointer apply(ExpressionPointer expression_smart, ExpressionPointer input, std::ostream& log) {
    switch (expression_smart.type) {
        case FUNCTION: return applyFunction(expression_smart.function(), input, log);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(expression_smart.functionBuiltIn(), input, log);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(expression_smart.functionDictionary(), input, log);
        case FUNCTION_LIST: return applyFunctionList(expression_smart.functionList(), input, log);
        default: throw std::runtime_error{"Expected function"};
    }
}

ExpressionPointer evaluateFunctionApplication(
    const FunctionApplication& function_application, ExpressionPointer environment, std::ostream& log
) {
    const auto function = lookupDictionary(environment, function_application.name->value);
    const auto evaluated_child = evaluate(function_application.child, environment, log);
    auto result = apply(function, evaluated_child, log);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, ExpressionPointer environment, std::ostream& log
) {
    auto result = lookupDictionary(environment, lookup_symbol.name->value);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateAtom(
    ExpressionPointer atom, ExpressionPointer, std::ostream& log
) {
    log << serialize(atom) << std::endl;
    return atom;
}

ExpressionPointer evaluate(
    ExpressionPointer expression,
    ExpressionPointer environment,
    std::ostream& log
) {
    switch (expression.type) {
        case CHARACTER: return evaluateAtom(expression, environment, log);
        case CONDITIONAL: return evaluateConditional(expression.conditional(), environment, log);
        case DICTIONARY: return evaluateDictionary(expression.dictionary(), environment, log);
        case FUNCTION: return evaluateFunction(expression.function(), environment, log);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression.functionDictionary(), environment, log);
        case FUNCTION_LIST: return evaluateFunctionList(expression.functionList(), environment, log);
        case LIST: return evaluateList(expression.list(), environment, log);
        case LOOKUP_CHILD: return evaluateLookupChild(expression.lookupChild(), environment, log);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(expression.functionApplication(), environment, log);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(expression.lookupSymbol(), environment, log);
        case NUMBER: return evaluateAtom(expression, environment, log);
        case STRING: return evaluateAtom(expression, environment, log);
        default: throw std::runtime_error{"Did not recognize expression to evaluate"};
    }
}
