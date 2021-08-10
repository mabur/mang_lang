#include "built_in_functions.h"

#include <memory>

#include "../expressions/Dictionary.h"
#include "../expressions/DictionaryElement.h"
#include "../expressions/FunctionBuiltIn.h"

#include "../factory.h"

#include "arithmetic.h"
#include "equality.h"
#include "list.h"
#include "logic.h"

ExpressionPointer makeDictionaryElement(
    std::string name,
    std::function<ExpressionPointer(ExpressionPointer)> function
) {
    return makeTypedDictionaryElement(
        new DictionaryElement{
            CodeRange{},
            ExpressionPointer{},
            std::make_shared<Name>(Name{CodeRange{}, ExpressionPointer{}, name}),
            makeFunctionBuiltIn(new FunctionBuiltIn{{}, {}, function}),
            1,
            0,
            0
        },
        NAMED_ELEMENT
    );
}

ExpressionPointer builtIns() {
    auto elements = DictionaryElements{};
    elements.push_back(makeDictionaryElement("min", arithmetic::min));
    elements.push_back(makeDictionaryElement("max", arithmetic::max));
    elements.push_back(makeDictionaryElement("add", arithmetic::add));
    elements.push_back(makeDictionaryElement("mul", arithmetic::mul));
    elements.push_back(makeDictionaryElement("sub", arithmetic::sub));
    elements.push_back(makeDictionaryElement("div", arithmetic::div));
    elements.push_back(makeDictionaryElement("less", arithmetic::less));
    elements.push_back(makeDictionaryElement("less_or_equal", arithmetic::less_or_equal));
    elements.push_back(makeDictionaryElement("round", arithmetic::round));
    elements.push_back(makeDictionaryElement("round_up", arithmetic::round_up));
    elements.push_back(makeDictionaryElement("round_down", arithmetic::round_down));
    elements.push_back(makeDictionaryElement("abs", arithmetic::abs));
    elements.push_back(makeDictionaryElement("sqrt", arithmetic::sqrt));
    elements.push_back(makeDictionaryElement("boolean", logic::boolean));
    elements.push_back(makeDictionaryElement("not", logic::logic_not));
    elements.push_back(makeDictionaryElement("and", logic::all));
    elements.push_back(makeDictionaryElement("or", logic::any));
    elements.push_back(makeDictionaryElement("none", logic::none));
    elements.push_back(makeDictionaryElement("equal", equality::equal));
    elements.push_back(makeDictionaryElement("unequal", equality::unequal));
    elements.push_back(makeDictionaryElement("empty", list_functions::empty));
    elements.push_back(makeDictionaryElement("prepend", list_functions::prepend));
    return makeDictionary(
        new Dictionary{CodeRange{}, ExpressionPointer{}, setContext(elements)}
    );
}
