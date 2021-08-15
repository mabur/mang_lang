#include "starts_with.h"

#include <vector>

#include "../parsing.h"

const auto KEYWORDS = std::vector<std::string>{"if", "then", "else", "from", "to"};

bool startsWithCharacter(CodeRange code) {
    return ::startsWith(code, '\'');
}

bool startsWithConditional(CodeRange code) {
    return isKeyword(code, "if");
}

bool startsWithDictionary(CodeRange code) {
    return ::startsWith(code, '{');
}

bool startsWithWhileElement(CodeRange code) {
    return isKeyword(code, "while");
}

bool startsWithEndElement(CodeRange code) {
    return isKeyword(code, "end");
}

bool startsWithFunction(CodeRange code) {
    return isKeyword(code, "in");
}

bool startsWithFunctionDictionary(CodeRange code) {
    if (!isKeyword(code, "in")) {
        return false;
    }
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    return ::startsWith(code, '{');
}

bool startsWithFunctionList(CodeRange code) {
    if (!isKeyword(code, "in")) {
        return false;
    }
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    return ::startsWith(code, '(');
}

bool startsWithList(CodeRange code) {
    return ::startsWith(code, '(');
}

bool startsWithLookupChild(CodeRange code) {
    if (!startsWithName(code)) {
        return false;
    }
    code = parseWhile(code, isNameCharacter);
    code = parseWhiteSpace(code);
    return ::startsWith(code, '@');
}

bool startsWithLookupFunction(CodeRange code) {
    if (!startsWithName(code)) {
        return false;
    }
    code = parseWhile(code, isNameCharacter);
    return ::startsWith(code, '!') || ::startsWith(code, '?');
}

bool startsWithLookupSymbol(CodeRange code) {
    return startsWithName(code);
}

bool startsWithName(CodeRange code) {
    if (isAnyKeyword(code, KEYWORDS)) {
        return false;
    }
    if (code.empty()) {
        return false;
    }
    const auto first = *code.begin();
    return isNameCharacter(first) && !isDigit(first);
}

bool startsWithNumber(CodeRange code) {
    return !code.empty() && (isSign(*code.begin()) || isDigit(*code.begin()));
}

bool startsWithString(CodeRange code) {
    return ::startsWith(code, '"');
}
