#include "expression.h"
#include <string.h>

#include "factory.h"

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

void Definitions::add(Expression key, Expression value) {
    for (auto& definition: definitions) {
        if (definition.name.index == key.index) {
            definition.expression = value;
            return;
        }
    }
    definitions.push_back({key, value});
}

bool Definitions::has(Expression key) const {
    for (const auto& definition: definitions) {
        if (definition.name.index == key.index) {
            return true;
        }
    }
    return false;
}


Expression Definitions::lookup(Expression key) const {
    for (const auto& definition: definitions) {
        if (definition.name.index == key.index) {
            return definition.expression;
        }
    }
    throw MissingSymbol(getName(key), "dictionary");
}
