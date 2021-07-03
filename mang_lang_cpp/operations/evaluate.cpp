#include "evaluate.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"

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
