#include "apply.h"

#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionBuiltIn.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"

ExpressionPointer applyFunction(
    const Function& function, ExpressionPointer input, std::ostream& log
) {
    auto middle = Dictionary({}, function.environment());
    middle.elements.push_back(std::make_shared<NamedElement>(
        function.range(), &middle, function.input_name, input, 0));
    auto output = function.body->evaluate(&middle, log);
    return output;
}

ExpressionPointer applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, ExpressionPointer input, std::ostream&
) {
    return function_built_in.function(*input);
}

ExpressionPointer applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return function_dictionary.body->evaluate(input.get(), log);
}

ExpressionPointer applyFunctionList(const FunctionList& function_list, ExpressionPointer input, std::ostream& log
) {
    auto middle = Dictionary(function_list.range(), function_list.environment());
    auto i = 0;
    for (auto list = input->list(); list; list = list->rest, ++i) {
        middle.elements.push_back(
            std::make_shared<NamedElement>(
                function_list.range(), &middle, function_list.input_names[i], list->first, i
            )
        );
    }
    auto output = function_list.body->evaluate(&middle, log);
    return output;
}
