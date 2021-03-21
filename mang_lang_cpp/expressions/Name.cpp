#include "Name.h"
#include <algorithm>

std::string Name::serialize() const {
    return value;
}

ExpressionPointer Name::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<Name>(begin(), end(), parent, value);
    log << result->serialize() << std::endl;
    return  result;
}

Name Name::parse(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, isLetter);
    it = std::find_if_not(it, last, isNameCharacter);
    return Name(first, it, nullptr, rawString(first, it));
}
