#include "DictionaryElement.h"

#include <cassert>

DictionaryElement::DictionaryElement(
    CodeRange range,
    const Expression* parent,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : Expression{range, parent},
    name{std::move(name)},
    expression{std::move(expression)},
    dictionary_index_{dictionary_index}
{}

NamedElement::NamedElement(
    CodeRange range,
    const Expression* parent,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : DictionaryElement{range, parent, std::move(name), std::move(expression), dictionary_index}
{}

WhileElement::WhileElement(
    CodeRange range,
    const Expression* parent,
    NamePointer name,
    ExpressionPointer expression,
    size_t dictionary_index
) : DictionaryElement{range, parent, std::move(name), std::move(expression), dictionary_index}
{}

EndElement::EndElement(CodeRange range, const Expression* parent)
    : DictionaryElement{range, parent, nullptr, nullptr, 0}
{}

std::string NamedElement::serialize() const {
    return name->serialize() + '=' + expression->serialize() + ',';
}

std::string WhileElement::serialize() const {
    return "while " + expression->serialize() + ',';
}

std::string EndElement::serialize() const {
    return "end,";
}

ExpressionPointer NamedElement::lookup(const std::string& s) const {
    if (name->value == s) {
        return expression;
    }
    return nullptr;
}

DictionaryElementPointer DictionaryElement::parse(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (isKeyword(code, "end")) {
        return EndElement::parse(code);
    }
    if (isKeyword(code, "while")) {
        return WhileElement::parse(code);
    }
    return NamedElement::parse(code);
}

DictionaryElementPointer NamedElement::parse(CodeRange code) {
    auto first = code.begin();
    auto name = Name::parse(code);
    code.first = name->end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = Expression::parse(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    code = parseOptionalCharacter(code, ',');
    return std::make_shared<NamedElement>(
        CodeRange{first, code.first}, nullptr, std::move(name), std::move(expression), 0
    );
}

DictionaryElementPointer WhileElement::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = Expression::parse(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    code = parseOptionalCharacter(code, ',');
    return std::make_shared<WhileElement>(
        CodeRange{first, code.first}, nullptr, nullptr, std::move(expression), 0
    );
}

DictionaryElementPointer EndElement::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    code = parseOptionalCharacter(code, ',');
    return std::make_shared<EndElement>(
        CodeRange{first, code.first}, nullptr
    );
}

void NamedElement::mutate(const Expression* parent, std::ostream& log,
    std::vector<DictionaryElementPointer>& elements) const {
    elements.at(dictionary_index_) = std::make_shared<NamedElement>(
        range(),
        nullptr,
        name,
        expression->evaluate(parent, log),
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

size_t WhileElement::jump(const Expression* parent, std::ostream& log) const {
    if (expression->evaluate(parent, log)->boolean()) {
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
