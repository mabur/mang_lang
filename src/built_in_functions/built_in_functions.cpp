#include "built_in_functions.h"

#include <string.h>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

static
Definition makeDefinitionBuiltIn(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, name, strlen(name)).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Expression builtIns() {
    size_t i  = 0;
    auto definitions = std::vector<Definition>{};
    definitions.push_back(makeDefinitionBuiltIn(i++, "clear",      container_functions::clear));
    definitions.push_back(makeDefinitionBuiltIn(i++, "put",        container_functions::put));
    definitions.push_back(makeDefinitionBuiltIn(i++, "take",       container_functions::take));
    definitions.push_back(makeDefinitionBuiltIn(i++, "drop",       container_functions::drop));
    definitions.push_back(makeDefinitionBuiltIn(i++, "get",        container_functions::get));
    definitions.push_back(makeDefinitionBuiltIn(i++, "add",        arithmetic::add));
    definitions.push_back(makeDefinitionBuiltIn(i++, "mul",        arithmetic::mul));
    definitions.push_back(makeDefinitionBuiltIn(i++, "sub",        arithmetic::sub));
    definitions.push_back(makeDefinitionBuiltIn(i++, "div",        arithmetic::div));
    definitions.push_back(makeDefinitionBuiltIn(i++, "mod",        arithmetic::mod));
    definitions.push_back(makeDefinitionBuiltIn(i++, "less",       arithmetic::less));
    definitions.push_back(makeDefinitionBuiltIn(i++, "round",      arithmetic::round));
    definitions.push_back(makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUp));
    definitions.push_back(makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDown));
    definitions.push_back(makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrt));
    definitions.push_back(makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumber));
    definitions.push_back(makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacter));
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    size_t i = 0;
    const auto definitions = std::vector<Definition>{
        makeDefinitionBuiltIn(i++, "clear",      container_functions::clearTyped),
        makeDefinitionBuiltIn(i++, "put",        container_functions::putTyped),
        makeDefinitionBuiltIn(i++, "take",       container_functions::takeTyped),
        makeDefinitionBuiltIn(i++, "drop",       container_functions::dropTyped),
        makeDefinitionBuiltIn(i++, "get",        container_functions::getTyped),
        makeDefinitionBuiltIn(i++, "add",        arithmetic::addTyped),
        makeDefinitionBuiltIn(i++, "mul",        arithmetic::mulTyped),
        makeDefinitionBuiltIn(i++, "sub",        arithmetic::subTyped),
        makeDefinitionBuiltIn(i++, "div",        arithmetic::divTyped),
        makeDefinitionBuiltIn(i++, "mod",        arithmetic::modTyped),
        makeDefinitionBuiltIn(i++, "less",       arithmetic::lessTyped),
        makeDefinitionBuiltIn(i++, "round",      arithmetic::roundTyped),
        makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUpTyped),
        makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDownTyped),
        makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrtTyped),
        makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumberTyped),
        makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacterTyped),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
