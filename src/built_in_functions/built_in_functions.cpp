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
    auto first = storage.definitions.count;
    makeDefinition({}, makeDefinitionBuiltIn(i++, "clear",      container_functions::clear));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "put",        container_functions::put));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "take",       container_functions::take));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "drop",       container_functions::drop));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "get",        container_functions::get));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "add",        arithmetic::add));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "mul",        arithmetic::mul));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "sub",        arithmetic::sub));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "div",        arithmetic::div));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "mod",        arithmetic::mod));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "less",       arithmetic::less));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round",      arithmetic::round));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUp));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDown));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrt));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumber));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacter));

    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    size_t i = 0;
    auto first = storage.definitions.count;
    makeDefinition({}, makeDefinitionBuiltIn(i++, "clear",      container_functions::clearTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "put",        container_functions::putTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "take",       container_functions::takeTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "drop",       container_functions::dropTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "get",        container_functions::getTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "add",        arithmetic::addTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "mul",        arithmetic::mulTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "sub",        arithmetic::subTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "div",        arithmetic::divTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "mod",        arithmetic::modTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "less",       arithmetic::lessTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round",      arithmetic::roundTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUpTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDownTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrtTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumberTyped));
    makeDefinition({}, makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacterTyped));
    
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
