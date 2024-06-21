#include "expression.h"
#include <string.h>

#include "factory.h"

UnexpectedExpression::UnexpectedExpression(
    ExpressionType type, const char* location)
    : std::runtime_error("Unexpected expression " + NAMES[type] + " for " + location)
{}

MissingSymbol::MissingSymbol(
    const std::string& symbol, Expression parent)
    : std::runtime_error("Cannot find symbol " + symbol + " in environment of type " + NAMES[parent.type])
{}

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
