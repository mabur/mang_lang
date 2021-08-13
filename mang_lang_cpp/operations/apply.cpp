#include "apply.h"

#include "list.h"

#include "../operations/evaluate.h"

#include "../factory.h"

ExpressionPointer applyFunction(
    const Function& function, ExpressionPointer input, std::ostream& log
) {

    const auto elements = DictionaryElements{
        makeNamedElement(
            new NamedElement{
                function.range,
                function.input_name,
                input,
                0
            }
        )
    };
    const auto middle = makeDictionary(
        new Dictionary{CodeRange{}, function.environment, elements}
    );
    return evaluate(function.body, middle, log);
}

ExpressionPointer applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, ExpressionPointer input, std::ostream&
) {
    return function_built_in.function(input);
}

ExpressionPointer applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, ExpressionPointer input, std::ostream& log
) {
    // TODO: pass along environment.
    return evaluate(function_dictionary.body, input, log);
}

ExpressionPointer applyFunctionList(const FunctionList& function_list, ExpressionPointer input, std::ostream& log
) {
    auto elements = DictionaryElements{};
    auto i = size_t{0};
    for (auto list = ::list(input); list; list = list->rest, ++i) {
        elements.push_back(
            makeNamedElement(
                new NamedElement{
                    function_list.range,
                    function_list.input_names[i],
                    list->first,
                    i
                }
            )
        );
    }
    const auto middle = makeDictionary(
        new Dictionary{
            function_list.range, function_list.environment, elements
        }
    );
    return evaluate(function_list.body, middle, log);
}

ExpressionPointer apply(ExpressionPointer expression_smart, ExpressionPointer input, std::ostream& log) {
    switch (expression_smart.type) {
        case FUNCTION: return applyFunction(expression_smart.function(), input, log);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(expression_smart.functionBuiltIn(), input, log);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(expression_smart.functionDictionary(), input, log);
        case FUNCTION_LIST: return applyFunctionList(expression_smart.functionList(), input, log);
        default: throw std::runtime_error{"Expected function"};
    }
}
