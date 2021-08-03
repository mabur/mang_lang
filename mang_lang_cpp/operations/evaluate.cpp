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

ExpressionPointer evaluateCharacter(
    const Character& character, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeCharacter(
        std::make_shared<Character>(character.range, environment, character.value)
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateConditional(
    const Conditional& conditional, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = boolean(evaluate(conditional.expression_if, environment, log)) ?
        evaluate(conditional.expression_then, environment, log) :
        evaluate(conditional.expression_else, environment, log);
    log << serialize(result) << std::endl;
    return result;
}

bool compareNameIndex(
    const DictionaryElementPointer& a, const DictionaryElementPointer& b
) {
    return a.get()->name_index_ < b.get()->name_index_;
}

size_t numNames(const DictionaryElements& elements) {
    if (elements.empty()) {
        return 0;
    }
    const auto max_element = std::max_element(
        elements.begin(), elements.end(), compareNameIndex
    );
    return 1 + max_element->get()->name_index_;
}

ExpressionPointer evaluateDictionary(
    const Dictionary& dictionary, const ExpressionPointer& environment, std::ostream& log
) {
    const auto num_names = numNames(dictionary.elements);
    const auto result = std::make_shared<Dictionary>(
        dictionary.range,
        environment,
        DictionaryElements(num_names, DictionaryElementPointer{})
    );
    const auto wrapped_result = makeDictionary(result);
    auto i = size_t{0};
    while (i < dictionary.elements.size()) {
        const auto element = dictionary.elements[i].get();
        if (element->type_ == NAMED_ELEMENT) {
            result->elements.at(element->name_index_) = makeTypedDictionaryElement(
                std::make_shared<DictionaryElement>(
                    element->range,
                    wrapped_result,
                    NAMED_ELEMENT,
                    element->name,
                    evaluate(element->expression, wrapped_result, log),
                    1,
                    0,
                    element->name_index_
                ),
                NAMED_ELEMENT
            );
            i += 1;
        }
        else if (element->type_ == WHILE_ELEMENT) {
            if (boolean(evaluate(element->expression, wrapped_result, log))) {
                i += 1;
            } else {
                i = element->end_index_ + 1;
            }
        }
        else if (element->type_ == END_ELEMENT) {
            i = element->while_index_;
        }
    }
    log << serialize(wrapped_result) << std::endl;
    return wrapped_result;
}

ExpressionPointer evaluateFunction(
    const Function& function, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeFunction(std::make_shared<Function>(
        function.range, environment, function.input_name, function.body
    ));
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeFunctionDictionary(
        std::make_shared<FunctionDictionary>(
            function_dictionary.range,
            environment,
            function_dictionary.input_names,
            function_dictionary.body
        )
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionList(
    const FunctionList& function_list, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeFunctionList(std::make_shared<FunctionList>(
        function_list.range, environment, function_list.input_names, function_list.body
    ));
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateList(
    const ExpressionPointer& list, const ExpressionPointer& environment, std::ostream& log
) {
    const auto operation = [&](const ExpressionPointer& expression) {
        return evaluate(expression, environment, log);
    };
    auto evaluated_elements = map(::list(list), operation);
    auto result = makeList(std::make_shared<List>(
        list->range, environment, std::move(evaluated_elements)));
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupChild(
    const LookupChild& lookup_child, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = lookup(
        evaluate(lookup_child.child, environment, log),
        lookup_child.name->value);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupFunction(
    const LookupFunction& lookup_function, const ExpressionPointer& environment, std::ostream& log
) {
    const auto function = lookup(environment, lookup_function.name->value);
    const auto evaluated_child = evaluate(lookup_function.child, environment, log);
    assert(evaluated_child);
    auto result = apply(function, evaluated_child, log);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = lookup(environment, lookup_symbol.name->value);
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateNumber(
    const Number& number, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeNumber(
        std::make_shared<Number>(number.range, environment, number.value)
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluateString(
    const ExpressionPointer& string, const ExpressionPointer& environment, std::ostream& log
) {
    auto result = makeString(
        std::make_shared<String>(string->range, environment, list(string))
    );
    log << serialize(result) << std::endl;
    return result;
}

ExpressionPointer evaluate(
    const ExpressionPointer& expression_smart,
    const ExpressionPointer& environment,
    std::ostream& log
) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == CHARACTER) {return evaluateCharacter(*dynamic_cast<const Character*>(expression), environment, log);}
    if (type == CONDITIONAL) {return evaluateConditional(*dynamic_cast<const Conditional*>(expression), environment, log);}
    if (type == DICTIONARY) {return evaluateDictionary(*dynamic_cast<const Dictionary*>(expression), environment, log);}
    if (type == FUNCTION) {return evaluateFunction(*dynamic_cast<const Function*>(expression), environment, log);}
    if (type == FUNCTION_DICTIONARY) {return evaluateFunctionDictionary(*dynamic_cast<const FunctionDictionary*>(expression), environment, log);}
    if (type == FUNCTION_LIST) {return evaluateFunctionList(*dynamic_cast<const FunctionList*>(expression), environment, log);}
    if (type == LIST) {return evaluateList(expression_smart, environment, log);}
    if (type == LOOKUP_CHILD) {return evaluateLookupChild(*dynamic_cast<const LookupChild*>(expression), environment, log);}
    if (type == LOOKUP_FUNCTION) {return evaluateLookupFunction(*dynamic_cast<const LookupFunction*>(expression), environment, log);}
    if (type == LOOKUP_SYMBOL) {return evaluateLookupSymbol(*dynamic_cast<const LookupSymbol*>(expression), environment, log);}
    if (type == NUMBER) {return evaluateNumber(*dynamic_cast<const Number*>(expression), environment, log);}
    if (type == STRING) {return evaluateString(expression_smart, environment, log);}
    throw std::runtime_error{"Did not recognize expression to evaluate"};
}
