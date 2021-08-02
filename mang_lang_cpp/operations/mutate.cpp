#include "mutate.h"

#include "../operations/evaluate.h"

DictionaryElements mutate(
    const DictionaryElementPointer& dictionary_element,
    const ExpressionPointer& environment,
    std::ostream& log,
    const DictionaryElements& elements) {
    if (dictionary_element->type_ == NAMED_ELEMENT) {
        auto new_elements = elements;
        new_elements.at(dictionary_element->dictionary_index_) = std::make_shared<DictionaryElement>(
            dictionary_element->range,
            environment,
            NAMED_ELEMENT,
            dictionary_element->name,
            evaluate(dictionary_element->expression, environment, log),
            dictionary_element->dictionary_index_
        );
        return new_elements;
    }
    if (dictionary_element->type_ == WHILE_ELEMENT) {
        return elements;
    }
    if (dictionary_element->type_ == END_ELEMENT) {
        return elements;
    }
    throw std::runtime_error{"Expected dictionary element"};
}