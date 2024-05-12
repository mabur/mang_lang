#include "built_in_functions.h"

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Definition makeDefinitionBuiltIn(const std::string& name, FunctionPointer function) {
    static size_t i = 0;
    return {
        {makeName(CodeRange{}, name).index, i++},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Definition makeDefinitionBuiltInTyped(const std::string& name, FunctionPointer function) {
    static size_t i = 0;
    return {
        {makeName(CodeRange{}, name).index, i++},
        makeFunctionBuiltIn(CodeRange{}, {function})
    };
}

Expression builtIns() {
    const auto definitions = std::vector<Definition>{
        makeDefinitionBuiltIn("clear",      container_functions::clear),
        makeDefinitionBuiltIn("put",        container_functions::put),
        makeDefinitionBuiltIn("take",       container_functions::take),
        makeDefinitionBuiltIn("drop",       container_functions::drop),
        makeDefinitionBuiltIn("get",        container_functions::get),
        makeDefinitionBuiltIn("add",        arithmetic::add),
        makeDefinitionBuiltIn("mul",        arithmetic::mul),
        makeDefinitionBuiltIn("sub",        arithmetic::sub),
        makeDefinitionBuiltIn("div",        arithmetic::div),
        makeDefinitionBuiltIn("mod",        arithmetic::mod),
        makeDefinitionBuiltIn("less",       arithmetic::less),
        makeDefinitionBuiltIn("round",      arithmetic::round),
        makeDefinitionBuiltIn("round_up",   arithmetic::round_up),
        makeDefinitionBuiltIn("round_down", arithmetic::round_down),
        makeDefinitionBuiltIn("sqrt",       arithmetic::sqrt),
        makeDefinitionBuiltIn("number",     arithmetic::ascii_number),
        makeDefinitionBuiltIn("character",  arithmetic::ascii_character),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    const auto definitions = std::vector<Definition>{
        makeDefinitionBuiltInTyped("clear",      container_functions::clearTyped),
        makeDefinitionBuiltInTyped("put",        container_functions::putTyped),
        makeDefinitionBuiltInTyped("take",       container_functions::takeTyped),
        makeDefinitionBuiltInTyped("drop",       container_functions::dropTyped),
        makeDefinitionBuiltInTyped("get",        container_functions::getTyped),
        makeDefinitionBuiltInTyped("add",        arithmetic::addTyped),
        makeDefinitionBuiltInTyped("mul",        arithmetic::mulTyped),
        makeDefinitionBuiltInTyped("sub",        arithmetic::subTyped),
        makeDefinitionBuiltInTyped("div",        arithmetic::divTyped),
        makeDefinitionBuiltInTyped("mod",        arithmetic::modTyped),
        makeDefinitionBuiltInTyped("less",       arithmetic::lessTyped),
        makeDefinitionBuiltInTyped("round",      arithmetic::roundTyped),
        makeDefinitionBuiltInTyped("round_up",   arithmetic::round_upTyped),
        makeDefinitionBuiltInTyped("round_down", arithmetic::round_downTyped),
        makeDefinitionBuiltInTyped("sqrt",       arithmetic::sqrtTyped),
        makeDefinitionBuiltInTyped("number",     arithmetic::FunctionCharacterToNumber{"number"}),
        makeDefinitionBuiltInTyped("character",  arithmetic::FunctionNumberToCharacter{"character"}),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
