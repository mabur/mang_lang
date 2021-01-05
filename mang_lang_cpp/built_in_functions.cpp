#include "built_in_functions.h"

#include "expressions/Dictionary.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/Number.h"

ExpressionPointer  add(const Expression& in){
    double sum = 0.0;
    for (const auto& element : in.list()) {
        sum += element->number();
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
