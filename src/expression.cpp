#include "expression.h"
#include <string.h>

StaticTypeError::StaticTypeError(
    ExpressionType type, const std::string& location)
    : std::runtime_error("Static type error " + NAMES[type] + " for " + location)
{}

UnexpectedExpression::UnexpectedExpression(
    ExpressionType type, const std::string& location)
    : std::runtime_error("Unexpected expression " + NAMES[type] + " for " + location)
{}

MissingSymbol::MissingSymbol(
    const std::string& symbol, const std::string& location)
    : std::runtime_error("Cannot find symbol " + symbol + " in " + location)
{}

MissingKey::MissingKey(
    const std::string& key)
    : std::runtime_error("Cannot find key " + key + " in table")
{}

bool Definitions::empty() const {
    return definitions.empty();
}

void Definitions::add(NamePointer key, Expression value) {
    for (auto& definition: definitions) {
        if (strcmp(definition.key, key) == 0) {
            definition.value = value;
            return;
        }
    }
    definitions.push_back({key, value});
}

bool Definitions::has(NamePointer key) const {
    for (const auto& definition: definitions) {
        if (strcmp(definition.key, key) == 0) {
            return true;
        }
    }
    return false;
}


Expression Definitions::lookup(NamePointer key) const {
    for (const auto& definition: definitions) {
        if (strcmp(definition.key, key) == 0) {
            return definition.value;
        }
    }
    throw MissingSymbol(key, "dictionary");
}

std::vector<EvaluatedDefinition> Definitions::sorted() const {
    return definitions;
}
