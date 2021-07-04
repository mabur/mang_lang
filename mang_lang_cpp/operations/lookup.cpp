#include "lookup.h"

#include "../expressions/Dictionary.h"

ExpressionPointer lookup(const Dictionary& dictionary, const std::string& name) {
    for (const auto& element : dictionary.elements) {
        if (element) {
            auto expression = element->lookup(name);
            if (expression) {
                return expression;
            }
        }
    }

    return lookupExpression(dictionary, name);
}

ExpressionPointer lookup(const NamedElement& named_element, const std::string& name) {
    if (named_element.name->value == name) {
        return named_element.expression;
    }
    return nullptr;
}

ExpressionPointer lookupExpression(const Expression& expression, const std::string& name) {
    if (!expression.environment()) {
        // TODO: define evaluation exception.
        throw ParseException("Cannot find symbol " + name);
    }
    return expression.environment()->lookup(name);
}
