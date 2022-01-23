#include "expression.h"

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
    auto& definition = definitions[key];
    if (definition.type == EMPTY) {
        order[key] = definitions.size() - 1;
    }
    definition = value;
}

Expression Definitions::lookup(const std::string& key) const {
    const auto iterator = definitions.find(key);
    return iterator == definitions.end() ? Expression{} : iterator->second;
}

std::vector<std::pair<std::string, Expression>> Definitions::sorted() const {
    auto items = std::vector<std::pair<std::string, Expression>>{definitions.size()};
    for (const auto& item : definitions) {
        const auto& key = item.first;
        const auto& index = order.find(key)->second;
        items[index] = item;
    }
    return items;
}
