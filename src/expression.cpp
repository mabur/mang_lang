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

const Expression* EvaluatedDictionary::optionalLookup(Expression key) const {
    for (const auto& definition: definitions) {
        if (definition.name.index == key.index) {
            return &definition.expression;
        }
    }
    return nullptr;
}

Expression EvaluatedDictionary::lookup(Expression key) const {
    const auto expression = optionalLookup(key);
    if (expression) {
        return *expression;
    }
    throw MissingSymbol(getName(key), "dictionary");
}
