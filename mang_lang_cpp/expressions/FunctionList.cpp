#include "FunctionList.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"
#include "DictionaryElement.h"

std::string FunctionList::serialize() const {
    auto result = std::string{};
    result += "from ";
    result += "(";
    for (const auto& name : input_names) {
        result += name.serialize();
        result += ",";
    }
    if (input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " to " + body->serialize();
    return result;
}

ExpressionPointer FunctionList::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<FunctionList>(range(), parent, input_names, body);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer FunctionList::apply(ExpressionPointer input, std::ostream& log) const {
    auto dictionary = std::make_shared<Dictionary>(range(), nullptr);
    auto i = 0;
    for (auto list = input->list(); list; list = list->rest, ++i) {
        dictionary->add(
            std::make_shared<DictionaryElement>(
                range(), nullptr, input_names[i], list->first
            )
        );
    }
    auto middle = dictionary->evaluate(parent(), log);
    auto output = body->evaluate(middle.get(), log);
    return output;
}

ExpressionPointer FunctionList::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "from");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<Name>{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = Name::parse(code);
        code.first = name.end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "to");
    auto body = Expression::parse(code);
    code.first = body->end();
    return std::make_shared<FunctionList>(
        CodeRange{first, code.begin()}, nullptr, input_names, body
    );
}

bool FunctionList::startsWith(CodeRange code) {
    if (!isKeyword(code, "from ")) {
        return false;
    }
    code = parseKeyword(code, "from");
    code = parseWhiteSpace(code);
    return ::startsWith(code, '(');
}
