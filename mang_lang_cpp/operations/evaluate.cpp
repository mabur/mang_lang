#include "evaluate.h"

#include "../expressions/Character.h"

ExpressionPointer evaluate(const Character& character, const Expression* environment, std::ostream& log
) {
    auto result = std::make_shared<Character>(character.range(), environment, character.value);
    log << result->serialize();
    return result;
}
