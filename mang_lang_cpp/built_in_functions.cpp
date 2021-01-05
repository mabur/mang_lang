#include "built_in_functions.h"

#include "expressions/Dictionary.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/Number.h"

ExpressionPointer  add(const Expression& in) {
    double result = 0.0;
    for (const auto& element : in.list()) {
        result += element->number();
    }
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, result);
}

ExpressionPointer  mul(const Expression& in) {
    double result = 1.0;
    for (const auto& element : in.list()) {
        result *= element->number();
    }
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, result);
}

ExpressionPointer sub(const Expression& in) {
    double result = in.list().at(0)->number() - in.list().at(1)->number();
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, result);
}

ExpressionPointer div2(const Expression& in) {
    double result = in.list().at(0)->number() / in.list().at(1)->number();
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, result);
}

ExpressionPointer builtIns() {
    auto environment = std::make_shared<Dictionary>(nullptr, nullptr, nullptr);
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "add"},
        std::make_shared<FunctionBuiltIn>(add)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "mul"},
        std::make_shared<FunctionBuiltIn>(mul)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "sub"},
        std::make_shared<FunctionBuiltIn>(sub)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "div"},
        std::make_shared<FunctionBuiltIn>(div2)
    ));
    return environment;
}
