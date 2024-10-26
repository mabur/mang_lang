#include "expression.h"
#include <string.h>

#include <carma/carma.h>

#include "exceptions.h"
#include "factory.h"

const Expression* EvaluatedDictionary::optionalLookup(size_t name) const {
    FOR_EACH(i, definitions) {
        if (storage.definitions.data[i].name.global_index == name) {
            return &storage.definitions.data[i].expression;
        }
    }
    return nullptr;
}

Expression EvaluatedDictionary::lookup(size_t name) const {
    const auto expression = optionalLookup(name);
    if (expression == nullptr) {
        const auto name_c = storage.names.at(name).c_str();
        throwException("Cannot find name %s in dictionary", name_c);
    }
    return *expression;
}
