#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Expression define(std::function<Expression(Expression)> function) {
    return makeFunctionBuiltIn(CodeRange{}, {function});
}

Expression builtIns() {
    auto definitions = Definitions{};
    definitions.add("clear", define(container_functions::clear));
    definitions.add("put", define(container_functions::put));
    definitions.add("take", define(container_functions::take));
    definitions.add("drop", define(container_functions::drop));
    definitions.add("get", define(container_functions::get));

    definitions.add("add", define(arithmetic::add));
    definitions.add("mul", define(arithmetic::mul));
    definitions.add("sub", define(arithmetic::sub));
    definitions.add("div", define(arithmetic::div));

    definitions.add("less", define(arithmetic::less));

    definitions.add("round", define(arithmetic::round));
    definitions.add("round_up", define(arithmetic::round_up));
    definitions.add("round_down", define(arithmetic::round_down));
    definitions.add("sqrt", define(arithmetic::sqrt));

    definitions.add("number", define(arithmetic::ascii_number));
    definitions.add("character", define(arithmetic::ascii_character));

    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    auto definitions = Definitions{};
    definitions.add("clear", define(container_functions::clearTyped));
    definitions.add("put", define(container_functions::putTyped));
    definitions.add("take", define(container_functions::takeTyped));
    definitions.add("drop", define(container_functions::dropTyped));
    definitions.add("get", define(container_functions::get));

    definitions.add("add", define(arithmetic::checkTypesNumberNumberToNumber));
    definitions.add("mul", define(arithmetic::checkTypesNumberNumberToNumber));
    definitions.add("sub", define(arithmetic::checkTypesNumberNumberToNumber));
    definitions.add("div", define(arithmetic::checkTypesNumberNumberToNumber));

    definitions.add("less", define(arithmetic::checkTypesNumberNumberToBoolean));

    definitions.add("round", define(arithmetic::checkTypesNumberToNumber));
    definitions.add("round_up", define(arithmetic::checkTypesNumberToNumber));
    definitions.add("round_down", define(arithmetic::checkTypesNumberToNumber));
    definitions.add("sqrt", define(arithmetic::checkTypesNumberToNumber));

    definitions.add("number", define(arithmetic::checkTypesCharacterToNumber));
    definitions.add("character", define(arithmetic::checkTypesNumberToCharacter));

    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
