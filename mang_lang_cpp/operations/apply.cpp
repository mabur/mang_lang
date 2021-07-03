#include "apply.h"

#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"

ExpressionPointer apply(
    const Function& function, ExpressionPointer input, std::ostream& log
) {
    auto middle = Dictionary({}, function.environment());
    middle.elements.push_back(std::make_shared<NamedElement>(
        function.range(), &middle, function.input_name, input, 0));
    auto output = function.body->evaluate(&middle, log);
    return output;
}

ExpressionPointer apply(
    const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return function_dictionary.body->evaluate(input.get(), log);
}
