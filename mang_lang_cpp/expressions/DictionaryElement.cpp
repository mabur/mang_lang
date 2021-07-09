#include "DictionaryElement.h"

#include <cassert>

#include "../operations/serialize.h"
#include "../operations/lookup.h"

DictionaryElement::DictionaryElement(
    CodeRange range,
    const Expression* environment,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : Expression{range, environment, DICTIONARY_ELEMENT},
    name{std::move(name)},
    expression{std::move(expression)},
    dictionary_index_{dictionary_index}
{}

NamedElement::NamedElement(
    CodeRange range,
    const Expression* environment,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : DictionaryElement{range, environment, std::move(name), std::move(expression), dictionary_index}
{}

WhileElement::WhileElement(
    CodeRange range,
    const Expression* environment,
    ExpressionPointer expression
) : DictionaryElement{range, environment, nullptr, std::move(expression), 0}
{}

EndElement::EndElement(CodeRange range, const Expression* environment)
    : DictionaryElement{range, environment, nullptr, nullptr, 0}
{}

std::string NamedElement::serialize() const {
    return ::serialize(*this);
}

std::string WhileElement::serialize() const {
    return ::serialize(*this);
}

std::string EndElement::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer NamedElement::lookup(const std::string& s) const {
    return ::lookupNamedElement(*this, s);
}

void NamedElement::mutate(const Expression* environment, std::ostream& log,
    std::vector<DictionaryElementPointer>& elements) const {
    elements.at(dictionary_index_) = std::make_shared<NamedElement>(
        range(),
        environment,
        name,
        expression->evaluate(environment, log),
        dictionary_index_
    );
}

void WhileElement::mutate(const Expression*, std::ostream&,
    std::vector<DictionaryElementPointer>&) const {
}

void EndElement::mutate(const Expression*, std::ostream&,
    std::vector<DictionaryElementPointer>&) const {
}

size_t NamedElement::jump(const Expression*, std::ostream&) const {
    return jump_true;
}

size_t WhileElement::jump(const Expression* environment, std::ostream& log) const {
    if (expression->evaluate(environment, log)->boolean()) {
        return jump_true;
    }
    return jump_false;
}

size_t EndElement::jump(const Expression*, std::ostream&) const {
    return jump_true;
}

bool DictionaryElement::isWhile() const {return !name && expression;}
bool DictionaryElement::isEnd() const {return !name && !expression;}
bool DictionaryElement::isSymbolDefinition() const {return name && expression;}

void setContext(DictionaryElements& elements) {
    // Forward pass to set backward jumps:
    auto while_positions = std::vector<size_t>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->isWhile()) {
            while_positions.push_back(i);
        }
        if (element->isEnd()) {
            element->jump_true = while_positions.back() - i;
            while_positions.pop_back();
        }
    }
    if (!while_positions.empty()) {
        throw ParseException("More while than end", elements.front()->begin());
    }
    // Backward pass to set forward jumps:
    auto end_positions = std::vector<size_t>{};
    for (size_t i = elements.size() - 1; i < elements.size(); --i) {
        auto& element = elements[i];
        if (element->isEnd()) {
            end_positions.push_back(i);
        }
        if (element->isWhile()) {
            element->jump_false = end_positions.back() - i + 1;
            end_positions.pop_back();
        }
    }
    if (!end_positions.empty()) {
        throw ParseException("Fewer while than end", elements.front()->begin());
    }
    // Forward pass to set dictionary_index_:
    auto names = std::vector<std::string>{};
    for (size_t i = 0; i < elements.size(); ++i) {
        auto& element = elements[i];
        if (element->isSymbolDefinition()) {
            const auto name = element->name->value;
            const auto it = std::find(names.begin(), names.end(), name);
            element->dictionary_index_ = std::distance(names.begin(), it);
            if (it == names.end()) {
                names.push_back(name);
            }
        }
    }
}

bool compareDictionaryIndex(
    const DictionaryElementPointer& a, const DictionaryElementPointer& b) {
    return a->dictionary_index_ < b->dictionary_index_;
}

size_t numNames(const DictionaryElements& elements) {
    if (elements.empty()) {
        return 0;
    }
    const auto max_element = std::max_element(
        elements.begin(), elements.end(), compareDictionaryIndex
    );
    return 1 + max_element->get()->dictionary_index_;
}
