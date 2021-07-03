#include "evaluate.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"

ExpressionPointer evaluate(
    const Character& character, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Character>(character.range(), environment, character.value);
    log << result->serialize();
    return result;
}

ExpressionPointer evaluate(
    const Conditional& conditional, const Expression* environment, std::ostream& log
) {
    auto result = conditional.expression_if->evaluate(environment, log)->boolean() ?
        conditional.expression_then->evaluate(environment, log) :
        conditional.expression_else->evaluate(environment, log);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer evaluate(
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
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer evaluate(
    const Function& function, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Function>(
        function.range(), environment, function.input_name, function.body
    );
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer evaluate(
    const FunctionDictionary& function_dictionary, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<FunctionDictionary>(
        function_dictionary.range(), environment, function_dictionary.input_names, function_dictionary.body
    );
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer evaluate(
    const FunctionList& function_list, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<FunctionList>(
        function_list.range(), environment, function_list.input_names, function_list.body
    );
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer evaluate(const List& list, const Expression* environment, std::ostream& log
) {
    const auto operation = [&](const ExpressionPointer& expression) {
        return expression->evaluate(environment, log);
    };
    auto evaluated_elements = map(list.list(), operation);
    auto result = std::make_shared<List>(
        list.range(), environment, std::move(evaluated_elements));
    log << result->serialize() << std::endl;
    return result;
}
