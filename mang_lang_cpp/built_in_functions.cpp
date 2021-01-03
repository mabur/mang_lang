#include "built_in_functions.h"

#include <stdexcept>

#include "expressions/Dictionary.h"
#include "expressions/Expression.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/List.h"
#include "expressions/Number.h"

// TODO: Put the casts as virtual member functions in Expression?
ExpressionPointer  add(const Expression& in){
    const auto list = dynamic_cast<const List*>(&in);
    if (!list) {
        throw std::runtime_error{"Expected list"};
    }
    double sum = 0.0;
    for (const auto& element : list->elements) {
        const auto number = dynamic_cast<const Number*>(element.get());
        if (!number) {
            throw std::runtime_error{"Expected number"};
        }
        sum += number->value;
    }
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, sum);
}

ExpressionPointer builtIns() {
    auto environment = std::make_shared<Dictionary>(nullptr, nullptr, nullptr);
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "add"},
        std::make_shared<FunctionBuiltIn>(add)
    ));
    return environment;
}
