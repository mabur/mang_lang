#pragma once

struct CodeRange;

bool startsWithCharacter(CodeRange code);
bool startsWithDictionary(CodeRange code);
bool startsWithEndStatement(CodeRange code);
bool startsWithFunction(CodeRange code);
bool startsWithFunctionDictionary(CodeRange code);
bool startsWithFunctionList(CodeRange code);
bool startsWithList(CodeRange code);
bool startsWithLookupChild(CodeRange code);
bool startsWithLookupFunction(CodeRange code);
bool startsWithLookupSymbol(CodeRange code);
bool startsWithName(CodeRange code);
bool startsWithNumber(CodeRange code);
bool startsWithString(CodeRange code);
