#include "evaluate.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"

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
