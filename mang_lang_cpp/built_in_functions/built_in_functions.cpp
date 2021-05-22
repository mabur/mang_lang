#include "built_in_functions.h"

#include <memory>

#include "../expressions/Dictionary.h"
#include "../expressions/DictionaryElement.h"
#include "../expressions/FunctionBuiltIn.h"

#include "arithmetic.h"
#include "equality.h"
#include "list.h"
#include "logic.h"

ExpressionPointer makeDictionaryElement(
    std::string name,
    std::function<ExpressionPointer(const Expression&)> function
) {
    return std::make_shared<DictionaryElement>(
        CodeRange{},
        nullptr,
        std::make_shared<Name>(CodeRange{}, nullptr, name),
        std::make_shared<FunctionBuiltIn>(function)
    );
}

ExpressionPointer builtIns() {
    auto elements = std::vector<ExpressionPointer>{};
    elements.push_back(makeDictionaryElement("min", arithmetic::min));
    elements.push_back(makeDictionaryElement("max", arithmetic::max));
    elements.push_back(makeDictionaryElement("add", arithmetic::add));
    elements.push_back(makeDictionaryElement("mul", arithmetic::mul));
    elements.push_back(makeDictionaryElement("sub", arithmetic::sub));
    elements.push_back(makeDictionaryElement("div", arithmetic::div));
    elements.push_back(makeDictionaryElement("abs", arithmetic::abs));
    elements.push_back(makeDictionaryElement("sqrt", arithmetic::sqrt));
    elements.push_back(makeDictionaryElement("boolean", logic::boolean));
    elements.push_back(makeDictionaryElement("not", logic::logic_not));
    elements.push_back(makeDictionaryElement("all", logic::all));
    elements.push_back(makeDictionaryElement("any", logic::any));
    elements.push_back(makeDictionaryElement("none", logic::none));
    elements.push_back(makeDictionaryElement("equal", equality::equal));
    elements.push_back(makeDictionaryElement("unequal", equality::unequal));
    elements.push_back(makeDictionaryElement("first", list_functions::first));
    elements.push_back(makeDictionaryElement("rest", list_functions::rest));
    elements.push_back(makeDictionaryElement("reverse", list_functions::reverse));
    elements.push_back(makeDictionaryElement("prepend", list_functions::prepend));
    auto environment = std::make_shared<Dictionary>(CodeRange{}, nullptr);
    environment->elements = elements;
    return environment;
}
