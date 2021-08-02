#include "jump.h"

#include "../operations/boolean.h"
#include "../operations/evaluate.h"

size_t jump(
    const DictionaryElementPointer& dictionary_element,
    const ExpressionPointer& environment,
    std::ostream& log
) {
    if (dictionary_element->type_ == NAMED_ELEMENT) {
        return dictionary_element->jump_true;
    }
    if (dictionary_element->type_ == WHILE_ELEMENT) {
        if (boolean(evaluate(dictionary_element->expression, environment, log))) {
            return dictionary_element->jump_true;
        }
        return dictionary_element->jump_false;
    }
    if (dictionary_element->type_ == END_ELEMENT) {
        return dictionary_element->jump_true;
    }
    throw std::runtime_error{"Expected dictionary element"};
}
