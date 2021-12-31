#pragma once

struct CodeRange;

bool startsWithFunction(CodeRange code);
bool startsWithFunctionDictionary(CodeRange code);
bool startsWithFunctionList(CodeRange code);
bool startsWithLookupChild(CodeRange code);
bool startsWithLookupFunction(CodeRange code);
bool startsWithLookupSymbol(CodeRange code);
bool startsWithName(CodeRange code);
bool startsWithNumber(CodeRange code);
