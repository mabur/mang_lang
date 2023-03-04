#include "expression.h"

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

bool Definitions::empty() const {
    return definitions.empty();
}

void Definitions::add(const std::string& key, Expression value) {
    for (auto& definition: definitions) {
        if (definition.first == key) {
            definition.second = value;
            return;
        }
    }
    definitions.push_back({key, value});
}

bool Definitions::has(const std::string& key) const {
    for (const auto& definition: definitions) {
        if (definition.first == key) {
            return true;
        }
    }
    return false;
}


Expression Definitions::lookup(const std::string& key) const {
    for (const auto& definition: definitions) {
        if (definition.first == key) {
            return definition.second;
        }
    }
    throw MissingSymbol(key, "dictionary");
}

std::vector<std::pair<std::string, Expression>> Definitions::sorted() const {
    return definitions;
}
