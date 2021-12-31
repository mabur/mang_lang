#pragma once

struct CodeRange;

bool startsWithLookupChild(CodeRange code);
bool startsWithLookupFunction(CodeRange code);
bool startsWithLookupSymbol(CodeRange code);
bool startsWithName(CodeRange code);
bool startsWithNumber(CodeRange code);
