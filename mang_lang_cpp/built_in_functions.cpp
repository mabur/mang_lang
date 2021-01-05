#include "built_in_functions.h"

#include <cmath>

#include "expressions/Dictionary.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/Number.h"

namespace builtin {

ExpressionPointer makeNumber(const Expression& in, double x) {
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, x);
}

ExpressionPointer add(const Expression& in) {
    auto result = 0.0;
    for (const auto& element : in.list()) {
        result += element->number();
    }
    return makeNumber(in, result);
}

ExpressionPointer  mul(const Expression& in) {
    auto result = 1.0;
    for (const auto& element : in.list()) {
        result *= element->number();
    }
    return makeNumber(in, result);
}

ExpressionPointer sub(const Expression& in) {
    const auto result = in.list().at(0)->number() - in.list().at(1)->number();
    return makeNumber(in, result);
}

ExpressionPointer div(const Expression& in) {
    const auto result = in.list().at(0)->number() / in.list().at(1)->number();
    return makeNumber(in, result);
}

ExpressionPointer abs(const Expression& in) {
    const auto result = std::fabs(in.number());
    return makeNumber(in, result);
}

ExpressionPointer sqrt(const Expression& in) {
    const auto result = std::sqrt(in.number());
    return makeNumber(in, result);
}

}

ExpressionPointer builtIns() {
    auto environment = std::make_shared<Dictionary>(nullptr, nullptr, nullptr);
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "add"},
        std::make_shared<FunctionBuiltIn>(builtin::add)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "mul"},
        std::make_shared<FunctionBuiltIn>(builtin::mul)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "sub"},
        std::make_shared<FunctionBuiltIn>(builtin::sub)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "div"},
        std::make_shared<FunctionBuiltIn>(builtin::div)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "abs"},
        std::make_shared<FunctionBuiltIn>(builtin::abs)
    ));
    environment->add(DictionaryElement(
        Name{nullptr, nullptr, nullptr, "sqrt"},
        std::make_shared<FunctionBuiltIn>(builtin::sqrt)
    ));
    return environment;
}
