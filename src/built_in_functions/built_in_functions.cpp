#include "built_in_functions.h"

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Definition makeDefinitionBuiltIn(const std::string& name, FunctionPointer function) {
    // TODO: do not have i as static, to support initializing manglang multiple times.
    static size_t i = 0;
    return Definition{
        {makeName(CodeRange{}, name).index, i++},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Definition makeDefinitionBuiltInTyped(const std::string& name, FunctionPointer function) {
    // TODO: do not have i as static, to support initializing manglang multiple times.
    static size_t i = 0;
    return Definition{
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
        makeDefinitionBuiltIn("round_up",   arithmetic::roundUp),
        makeDefinitionBuiltIn("round_down", arithmetic::roundDown),
        makeDefinitionBuiltIn("sqrt",       arithmetic::sqrt),
        makeDefinitionBuiltIn("number",     arithmetic::asciiNumber),
        makeDefinitionBuiltIn("character",  arithmetic::asciiCharacter),
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
        makeDefinitionBuiltInTyped("round_up",   arithmetic::roundUpTyped),
        makeDefinitionBuiltInTyped("round_down", arithmetic::roundDownTyped),
        makeDefinitionBuiltInTyped("sqrt",       arithmetic::sqrtTyped),
        makeDefinitionBuiltInTyped("number",     arithmetic::asciiNumberTyped),
        makeDefinitionBuiltInTyped("character",  arithmetic::asciiCharacterTyped),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
