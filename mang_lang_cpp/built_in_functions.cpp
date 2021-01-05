#include "built_in_functions.h"

#include <cmath>
#include <limits>

#include "expressions/Dictionary.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/Number.h"

namespace builtin {

ExpressionPointer makeNumber(const Expression& in, double x) {
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, x);
}

ExpressionPointer min(const Expression& in) {
    auto result = std::numeric_limits<double>::infinity();
    for (const auto& element : in.list()) {
        result = std::min(result, element->number());
    }
    return makeNumber(in, result);
}

ExpressionPointer max(const Expression& in) {
    auto result = -std::numeric_limits<double>::infinity();
    for (const auto& element : in.list()) {
        result = std::max(result, element->number());
    }
    return makeNumber(in, result);
}

ExpressionPointer add(const Expression& in) {
    auto result = 0.0;
    for (const auto& element : in.list()) {
        result += element->number();
    }
    return makeNumber(in, result);
}

ExpressionPointer mul(const Expression& in) {
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

DictionaryElement makeDictionaryElement(
    std::string name,
    std::function<ExpressionPointer(const Expression&)> function
) {
    return DictionaryElement(
        Name{nullptr, nullptr, nullptr, name},
        std::make_shared<FunctionBuiltIn>(function)
    );
}

ExpressionPointer builtIns() {
    auto environment = std::make_shared<Dictionary>(nullptr, nullptr, nullptr);
    environment->add(makeDictionaryElement("min", builtin::min));
    environment->add(makeDictionaryElement("max", builtin::max));
    environment->add(makeDictionaryElement("add", builtin::add));
    environment->add(makeDictionaryElement("mul", builtin::mul));
    environment->add(makeDictionaryElement("sub", builtin::sub));
    environment->add(makeDictionaryElement("div", builtin::div));
    environment->add(makeDictionaryElement("abs", builtin::abs));
    environment->add(makeDictionaryElement("sqrt", builtin::sqrt));
    return environment;
}
