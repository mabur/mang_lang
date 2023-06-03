#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Expression define(std::function<Expression(Expression)> function) {
    return makeFunctionBuiltIn(CodeRange{}, {function});
}

Expression builtIns() {
    const auto definitions = Definitions{{
        {"clear",      define(container_functions::clear)},
        {"put",        define(container_functions::put)},
        {"take",       define(container_functions::take)},
        {"drop",       define(container_functions::drop)},
        {"get",        define(container_functions::get)},
        {"add",        define(arithmetic::add)},
        {"mul",        define(arithmetic::mul)},
        {"sub",        define(arithmetic::sub)},
        {"div",        define(arithmetic::div)},
        {"mod",        define(arithmetic::mod)},
        {"less",       define(arithmetic::less)},
        {"round",      define(arithmetic::round)},
        {"round_up",   define(arithmetic::round_up)},
        {"round_down", define(arithmetic::round_down)},
        {"sqrt",       define(arithmetic::sqrt)},
        {"number",     define(arithmetic::ascii_number)},
        {"character",  define(arithmetic::ascii_character)},
    }};
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

    definitions.add("add", define(arithmetic::FunctionNumberNumberToNumber{"add"}));
    definitions.add("mul", define(arithmetic::FunctionNumberNumberToNumber{"mul"}));
    definitions.add("sub", define(arithmetic::FunctionNumberNumberToNumber{"sub"}));
    definitions.add("div", define(arithmetic::FunctionNumberNumberToNumber{"div"}));
    definitions.add("mod", define(arithmetic::FunctionNumberNumberToNumber{"mod"}));

    definitions.add("less", define(arithmetic::FunctionNumberNumberToBoolean{"less"}));

    definitions.add("round", define(arithmetic::FunctionNumberToNumber{"round"}));
    definitions.add("round_up", define(arithmetic::FunctionNumberToNumber{"round_up"}));
    definitions.add("round_down", define(arithmetic::FunctionNumberToNumber{"round_down"}));
    definitions.add("sqrt", define(arithmetic::FunctionNumberToNumber{"sqrt"}));

    definitions.add("number", define(arithmetic::FunctionCharacterToNumber{"number"}));
    definitions.add("character", define(arithmetic::FunctionNumberToCharacter{"character"}));

    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
