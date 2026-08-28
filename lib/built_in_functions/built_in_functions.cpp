#include "built_in_functions.h"

#include <string.h>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

const BuiltInEntry BUILT_IN_ENTRIES[] = {
    {"clear",      container_functions::clear, container_functions::clearTyped},
    {"put",        container_functions::put,   container_functions::putTyped},
    {"take",       container_functions::take,  container_functions::takeTyped},
    {"drop",       container_functions::drop,  container_functions::dropTyped},
    {"get",        container_functions::get,   container_functions::getTyped},
    {"add",        arithmetic::add,            arithmetic::add},
    {"mul",        arithmetic::mul,            arithmetic::mul},
    {"sub",        arithmetic::sub,            arithmetic::sub},
    {"div",        arithmetic::div,            arithmetic::div},
    {"mod",        arithmetic::mod,            arithmetic::mod},
    {"less",       arithmetic::less,           arithmetic::less},
    {"round",      arithmetic::round,          arithmetic::round},
    {"round_up",   arithmetic::roundUp,        arithmetic::roundUp},
    {"round_down", arithmetic::roundDown,      arithmetic::roundDown},
    {"sqrt",       arithmetic::sqrt,           arithmetic::sqrt},
    {"number",     arithmetic::asciiNumber,    arithmetic::asciiNumber},
    {"character",  arithmetic::asciiCharacter, arithmetic::asciiCharacter},
};

const size_t BUILT_IN_ENTRIES_COUNT = sizeof(BUILT_IN_ENTRIES) / sizeof(BUILT_IN_ENTRIES[0]);

const BuiltInEntry* findBuiltIn(size_t name_index) {
    const auto name_text = storage.names.data + name_index;
    for (const auto& entry : BUILT_IN_ENTRIES) {
        if (strcmp(name_text, entry.name) == 0) {
            return &entry;
        }
    }
    return nullptr;
}

static
Definition makeDefinitionBuiltIn(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, name, strlen(name)).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Expression builtIns() {
    auto first = storage.definitions.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        makeDefinition({}, makeDefinitionBuiltIn(i, entry.name, entry.function));
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    auto first = storage.definitions.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        makeDefinition({}, makeDefinitionBuiltIn(i, entry.name, entry.function_types));
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
