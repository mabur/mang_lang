#pragma once

#include <stdint.h>

struct CodeRange;

CodeRange firstPart(CodeRange whole, CodeRange last_part);
CodeRange lastPart(CodeRange whole, CodeRange middle_part);

const char* describeLocation(CodeRange code);

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

bool isKeyword(CodeRange code, const char* word);

bool startsWith(CodeRange code, const char* word);
bool startsWith(CodeRange code, char c);

CodeRange parseWhiteSpace(CodeRange code);
CodeRange parseCharacter(CodeRange code);
CodeRange parseCharacter(CodeRange code, char expected);
CodeRange parseKeyword(CodeRange code, const char* keyword);
CodeRange parseRawName(CodeRange code);

struct ParseResult {
    double value;
    uint16_t count;
};

ParseResult parseDecimal(CodeRange code);
