#include "evaluate.h"

#include <cassert>

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"
#include "../expressions/LookupChild.h"
#include "../expressions/LookupFunction.h"
#include "../expressions/LookupSymbol.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "apply.h"
#include "list.h"
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

bool compareNameIndex(
    ExpressionPointer a, ExpressionPointer b
) {
    return a.dictionaryElement().name_index_ < b.dictionaryElement().name_index_;
}

size_t numNames(const DictionaryElements& elements) {
    if (elements.empty()) {
        return 0;
    }
    const auto max_element = std::max_element(
        elements.begin(), elements.end(), compareNameIndex
    );
    return 1 + max_element->dictionaryElement().name_index_;
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
        const auto element = dictionary.elements[i].dictionaryElement();
        if (type == NAMED_ELEMENT) {
            result->elements.at(element.name_index_) = makeTypedDictionaryElement(
                std::make_shared<DictionaryElement>(DictionaryElement{
                    element.range,
                    wrapped_result,
                    element.name,
                    evaluate(element.expression, wrapped_result, log),
                    1,
                    0,
                    element.name_index_
                }),
                NAMED_ELEMENT
            );
            i += 1;
        }
        else if (type == WHILE_ELEMENT) {
            if (boolean(evaluate(element.expression, wrapped_result, log))) {
                i += 1;
            } else {
                i = element.end_index_ + 1;
            }
        }
        else if (type == END_ELEMENT) {
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
        std::make_shared<FunctionDictionary>(FunctionDictionary{
            function_dictionary.range,
            environment,
            function_dictionary.input_names,
            function_dictionary.body
        })
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionList(
    const FunctionList& function_list, ExpressionPointer environment, std::ostream& log
) {
    auto result = makeFunctionList(std::make_shared<FunctionList>(FunctionList{
        function_list.range, environment, function_list.input_names, function_list.body
    }));
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateList(
    const List& list, ExpressionPointer environment, std::ostream& log
) {
    const auto operation = [&](ExpressionPointer expression) {
        return evaluate(expression, environment, log);
    };
    auto evaluated_elements = map(list.elements, operation);
    auto result = makeList(std::make_shared<List>(List{
        list.range, environment, std::move(evaluated_elements)}));
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer lookupDictionary(const ExpressionPointer& expression, const std::string& name) {
    if (expression.type != DICTIONARY) {
        throw std::runtime_error("Cannot find symbol " + name);
    }
    const auto dictionary = expression.dictionary();
    for (const auto& element : dictionary.elements) {
        if (element) {
            const auto dictionary_element = element.dictionaryElement();
            if (dictionary_element.name->value == name) {
                return dictionary_element.expression;
            }
        }
    }
    return lookupDictionary(dictionary.environment, name);
}

ExpressionPointer evaluateLookupChild(
    const LookupChild& lookup_child, ExpressionPointer environment, std::ostream& log
) {
    auto result = lookup(
        evaluate(lookup_child.child, environment, log),
        lookup_child.name->value);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupFunction(
    const LookupFunction& lookup_function, ExpressionPointer environment, std::ostream& log
) {
    const auto function = lookupDictionary(environment, lookup_function.name->value);
    const auto evaluated_child = evaluate(lookup_function.child, environment, log);
    assert(evaluated_child);
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
    const auto type = expression.type;
    switch (type) {
        case CHARACTER: return evaluateAtom(expression, environment, log);
        case CONDITIONAL: return evaluateConditional(expression.conditional(), environment, log);
        case DICTIONARY: return evaluateDictionary(expression.dictionary(), environment, log);
        case FUNCTION: return evaluateFunction(expression.function(), environment, log);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression.functionDictionary(), environment, log);
        case FUNCTION_LIST: return evaluateFunctionList(expression.functionList(), environment, log);
        case LIST: return evaluateList(expression.list(), environment, log);
        case LOOKUP_CHILD: return evaluateLookupChild(expression.lookupChild(), environment, log);
        case LOOKUP_FUNCTION: return evaluateLookupFunction(expression.lookupFunction(), environment, log);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(expression.lookupSymbol(), environment, log);
        case NUMBER: return evaluateAtom(expression, environment, log);
        case STRING: return evaluateAtom(expression, environment, log);
        default: throw std::runtime_error{"Did not recognize expression to evaluate"};
    }
}
