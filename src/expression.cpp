#include "expression.h"
#include <string.h>

#include "exceptions.h"
#include "factory.h"

void throwUnexpectedExpressionException(
    ExpressionType type, const char* location) {
    throw std::runtime_error(
        "Unexpected expression " + NAMES[type] + " for " + location
    );
}

const Expression* EvaluatedDictionary::optionalLookup(size_t name) const {
    for (const auto& definition: definitions) {
        if (definition.name.global_index == name) {
            return &definition.expression;
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
