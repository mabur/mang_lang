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

#include "list.h"
#include "serialize.h"

ExpressionPointer evaluateCharacter(
    const Character& character, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Character>(character.range(), environment, character.value);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateConditional(
    const Conditional& conditional, const Expression* environment, std::ostream& log
) {
    auto result = conditional.expression_if->evaluate(environment, log)->boolean() ?
        conditional.expression_then->evaluate(environment, log) :
        conditional.expression_else->evaluate(environment, log);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateDictionary(
    const Dictionary& dictionary, const Expression* environment, std::ostream& log
) {
    const auto num_names = numNames(dictionary.elements);
    auto result = std::make_shared<Dictionary>(dictionary.range(), environment);
    result->elements = std::vector<DictionaryElementPointer>(num_names, nullptr);
    auto i = size_t{0};
    while (i < dictionary.elements.size()) {
        dictionary.elements[i]->mutate(result.get(), log, result->elements);
        i += dictionary.elements[i]->jump(result.get(), log);
    }
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateFunction(
    const Function& function, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Function>(
        function.range(), environment, function.input_name, function.body
    );
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<FunctionDictionary>(
        function_dictionary.range(), environment, function_dictionary.input_names, function_dictionary.body
    );
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateFunctionList(
    const FunctionList& function_list, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<FunctionList>(
        function_list.range(), environment, function_list.input_names, function_list.body
    );
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateList(
    const List& list, const Expression* environment, std::ostream& log
) {
    const auto operation = [&](const ExpressionPointer& expression) {
        return expression->evaluate(environment, log);
    };
    auto evaluated_elements = map(::list(&list), operation);
    auto result = std::make_shared<List>(
        list.range(), environment, std::move(evaluated_elements));
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupChild(
    const LookupChild& lookup_child, const Expression* environment, std::ostream& log
) {
    auto result = lookup_child.child->evaluate(environment, log)->lookup(lookup_child.name->value);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupFunction(
    const LookupFunction& lookup_function, const Expression* environment, std::ostream& log
) {
    const auto function = environment->lookup(lookup_function.name->value);
    const auto evaluated_child = lookup_function.child->evaluate(environment, log);
    assert(evaluated_child);
    auto result = function->apply(evaluated_child, log);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, const Expression* environment, std::ostream& log
) {
    auto result = environment->lookup(lookup_symbol.name->value);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateNumber(
    const Number& number, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Number>(number.range(), environment, number.value);
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluateString(
    const String& string, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<String>(string.range(), environment, list(&string));
    log << serialize(result.get()) << std::endl;
    return result;
}

ExpressionPointer evaluate(const Expression* expression, const Expression* environment, std::ostream& log) {
    if (expression->type_ == CHARACTER) {return evaluateCharacter(*dynamic_cast<const Character*>(expression), environment, log);}
    if (expression->type_ == CONDITIONAL) {return evaluateConditional(*dynamic_cast<const Conditional*>(expression), environment, log);}
    if (expression->type_ == DICTIONARY) {return evaluateDictionary(*dynamic_cast<const Dictionary*>(expression), environment, log);}
    if (expression->type_ == FUNCTION) {return evaluateFunction(*dynamic_cast<const Function*>(expression), environment, log);}
    if (expression->type_ == FUNCTION_DICTIONARY) {return evaluateFunctionDictionary(*dynamic_cast<const FunctionDictionary*>(expression), environment, log);}
    if (expression->type_ == FUNCTION_LIST) {return evaluateFunctionList(*dynamic_cast<const FunctionList*>(expression), environment, log);}
    if (expression->type_ == LIST) {return evaluateList(*dynamic_cast<const List*>(expression), environment, log);}
    if (expression->type_ == LOOKUP_CHILD) {return evaluateLookupChild(*dynamic_cast<const LookupChild*>(expression), environment, log);}
    if (expression->type_ == LOOKUP_FUNCTION) {return evaluateLookupFunction(*dynamic_cast<const LookupFunction*>(expression), environment, log);}
    if (expression->type_ == LOOKUP_SYMBOL) {return evaluateLookupSymbol(*dynamic_cast<const LookupSymbol*>(expression), environment, log);}
    if (expression->type_ == NUMBER) {return evaluateNumber(*dynamic_cast<const Number*>(expression), environment, log);}
    if (expression->type_ == STRING) {return evaluateString(*dynamic_cast<const String*>(expression), environment, log);}
    throw std::runtime_error{"Did not recognize expression to evaluate"};
}
