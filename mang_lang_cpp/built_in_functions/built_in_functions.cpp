#include "built_in_functions.h"

#include <memory>

#include "../expressions/Dictionary.h"
#include "../expressions/DictionaryElement.h"
#include "../expressions/FunctionBuiltIn.h"

#include "arithmetic.h"
#include "equality.h"
#include "list.h"
#include "logic.h"

DictionaryElementPointer makeDictionaryElement(
    std::string name,
    std::function<ExpressionPointer(const Expression&)> function,
    size_t dictionary_index
) {
    return std::make_shared<NamedElement>(
        CodeRange{},
        nullptr,
        std::make_shared<Name>(CodeRange{}, nullptr, name),
        std::make_shared<FunctionBuiltIn>(function),
        dictionary_index
    );
}

ExpressionPointer builtIns() {
    size_t i = 0;
    auto elements = std::vector<DictionaryElementPointer>{};
    elements.push_back(makeDictionaryElement("min", arithmetic::min, i++));
    elements.push_back(makeDictionaryElement("max", arithmetic::max, i++));
    elements.push_back(makeDictionaryElement("add", arithmetic::add, i++));
    elements.push_back(makeDictionaryElement("mul", arithmetic::mul, i++));
    elements.push_back(makeDictionaryElement("sub", arithmetic::sub, i++));
    elements.push_back(makeDictionaryElement("div", arithmetic::div, i++));
    elements.push_back(makeDictionaryElement("abs", arithmetic::abs, i++));
    elements.push_back(makeDictionaryElement("sqrt", arithmetic::sqrt, i++));
    elements.push_back(makeDictionaryElement("boolean", logic::boolean, i++));
    elements.push_back(makeDictionaryElement("not", logic::logic_not, i++));
    elements.push_back(makeDictionaryElement("all", logic::all, i++));
    elements.push_back(makeDictionaryElement("any", logic::any, i++));
    elements.push_back(makeDictionaryElement("none", logic::none, i++));
    elements.push_back(makeDictionaryElement("equal", equality::equal, i++));
    elements.push_back(makeDictionaryElement("unequal", equality::unequal, i++));
    elements.push_back(makeDictionaryElement("first", list_functions::first, i++));
    elements.push_back(makeDictionaryElement("rest", list_functions::rest, i++));
    elements.push_back(makeDictionaryElement("reverse", list_functions::reverse, i++));
    elements.push_back(makeDictionaryElement("prepend", list_functions::prepend, i++));
    auto environment = std::make_shared<Dictionary>(CodeRange{}, nullptr);
    environment->elements = elements;
    return environment;
}
