#include "apply.h"

#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionBuiltIn.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "list.h"

#include "../operations/evaluate.h"

#include "../factory.h"

ExpressionPointer applyFunction(
    const Function* function, ExpressionPointer input, std::ostream& log
) {

    const auto elements = DictionaryElements{
        makeTypedDictionaryElement(
            std::make_shared<DictionaryElement>(
                function->range,
                ExpressionPointer{},
                NAMED_ELEMENT,
                function->input_name,
                input,
                1,
                0,
                0
            ),
            NAMED_ELEMENT
        )
    };
    const auto middle = makeDictionary(
        std::make_shared<Dictionary>(CodeRange{}, function->environment, elements)
    );
    return evaluate(function->body, middle, log);
}

ExpressionPointer applyFunctionBuiltIn(
    const FunctionBuiltIn* function_built_in, ExpressionPointer input, std::ostream&
) {
    return function_built_in->function(input);
}

ExpressionPointer applyFunctionDictionary(
    const FunctionDictionary* function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary->body, input, log);
}

ExpressionPointer applyFunctionList(const FunctionList* function_list, ExpressionPointer input, std::ostream& log
) {
    auto elements = DictionaryElements{};
    auto i = 0;
    for (auto list = ::list(input); list; list = list->rest, ++i) {
        elements.push_back(
            makeTypedDictionaryElement(
                std::make_shared<DictionaryElement>(
                    function_list->range,
                    ExpressionPointer{},
                    NAMED_ELEMENT,
                    function_list->input_names[i],
                    list->first,
                    1,
                    0,
                    i
                ),
                NAMED_ELEMENT
            )
        );
    }
    const auto middle = makeDictionary(
        std::make_shared<Dictionary>(
            function_list->range, function_list->environment, elements
        )
    );
    return evaluate(function_list->body, middle, log);
}

ExpressionPointer apply(const ExpressionPointer& expression_smart, ExpressionPointer input, std::ostream& log) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == FUNCTION) {
        return applyFunction(dynamic_cast<const Function *>(expression), input, log);
    }
    if (type == FUNCTION_BUILT_IN) {
        return applyFunctionBuiltIn(dynamic_cast<const FunctionBuiltIn *>(expression), input, log);
    }
    if (type == FUNCTION_DICTIONARY) {
        return applyFunctionDictionary(dynamic_cast<const FunctionDictionary *>(expression), input, log);
    }
    if (type == FUNCTION_LIST) {
        return applyFunctionList(dynamic_cast<const FunctionList *>(expression), input, log);
    }
    throw std::runtime_error{"Expected function"};
}
