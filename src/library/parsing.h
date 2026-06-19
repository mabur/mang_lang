#pragma once

struct CodeRange;

CodeRange firstPart(CodeRange whole, CodeRange last_part);
CodeRange lastPart(CodeRange whole, CodeRange middle_part);

const char* describeLocation(CodeRange code);

bool isKeyword(CodeRange code, const char* word);

bool startsWithString(CodeRange code, const char* word);
bool startsWith(CodeRange code, char c);
bool startsWithDigit(CodeRange code);

CodeRange parseWhiteSpace(CodeRange code);
CodeRange parseCharacter(CodeRange code);
CodeRange parseKeyword(CodeRange code, const char* keyword);
CodeRange parseRawName(CodeRange code);

double parseDigitAsDouble(CodeRange code);
