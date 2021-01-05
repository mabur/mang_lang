#include "built_in_functions.h"

#include "../expressions/Dictionary.h"
#include "../expressions/FunctionBuiltIn.h"

#include "arithmetic.h"
#include "logic.h"

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
    environment->add(makeDictionaryElement("min", arithmetic::min));
    environment->add(makeDictionaryElement("max", arithmetic::max));
    environment->add(makeDictionaryElement("add", arithmetic::add));
    environment->add(makeDictionaryElement("mul", arithmetic::mul));
    environment->add(makeDictionaryElement("sub", arithmetic::sub));
    environment->add(makeDictionaryElement("div", arithmetic::div));
    environment->add(makeDictionaryElement("abs", arithmetic::abs));
    environment->add(makeDictionaryElement("sqrt", arithmetic::sqrt));

    environment->add(makeDictionaryElement("boolean", logic::boolean));
    return environment;
}
