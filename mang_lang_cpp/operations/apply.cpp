#include "apply.h"

#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionBuiltIn.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "list.h"

#include "../operations/evaluate.h"

ExpressionPointer applyFunction(
    const Function* function, ExpressionPointer input, std::ostream& log
) {
    auto middle = Dictionary({}, function->environment);
    middle.elements.push_back(std::make_shared<NamedElement>(
        function->range(), &middle, function->input_name, input, 0));
    auto output = evaluate(function->body.get(), &middle, log);
    return output;
}

ExpressionPointer applyFunctionBuiltIn(
    const FunctionBuiltIn* function_built_in, ExpressionPointer input, std::ostream&
) {
    return function_built_in->function(*input);
}

ExpressionPointer applyFunctionDictionary(
    const FunctionDictionary* function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary->body.get(), input.get(), log);
}

ExpressionPointer applyFunctionList(const FunctionList* function_list, ExpressionPointer input, std::ostream& log
) {
    auto middle = Dictionary(function_list->range(),
        function_list->environment);
    auto i = 0;
    for (auto list = ::list(input.get()); list; list = list->rest, ++i) {
        middle.elements.push_back(
            std::make_shared<NamedElement>(
                function_list->range(), &middle, function_list->input_names[i], list->first, i
            )
        );
    }
    auto output = evaluate(function_list->body.get(), &middle, log);
    return output;
}

ExpressionPointer apply(const Expression* expression, ExpressionPointer input, std::ostream& log) {
    if (expression->type_ == FUNCTION) {
        return applyFunction(dynamic_cast<const Function *>(expression), input, log);
    }
    if (expression->type_ == FUNCTION_BUILT_IN) {
        return applyFunctionBuiltIn(dynamic_cast<const FunctionBuiltIn *>(expression), input, log);
    }
    if (expression->type_ == FUNCTION_DICTIONARY) {
        return applyFunctionDictionary(dynamic_cast<const FunctionDictionary *>(expression), input, log);
    }
    if (expression->type_ == FUNCTION_LIST) {
        return applyFunctionList(dynamic_cast<const FunctionList *>(expression), input, log);
    }
    throw std::runtime_error{"Expected function"};
}
