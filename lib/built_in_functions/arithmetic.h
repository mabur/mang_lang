#pragma once

struct Expression;

// Binary functions, taking a tuple of two numbers:
Expression builtInAdd(Expression in);
Expression builtInMul(Expression in);
Expression builtInSub(Expression in);
Expression builtInDiv(Expression in);
Expression builtInMod(Expression in);
Expression builtInLess(Expression in);

// The same binary functions, taking the two numbers directly:
Expression builtInAdd2(Expression left, Expression right);
Expression builtInMul2(Expression left, Expression right);
Expression builtInSub2(Expression left, Expression right);
Expression builtInDiv2(Expression left, Expression right);
Expression builtInMod2(Expression left, Expression right);
Expression builtInLess2(Expression left, Expression right);

Expression builtInSqrt(Expression in);
Expression builtInRound(Expression in);
Expression builtInRoundUp(Expression in);
Expression builtInRoundDown(Expression in);

Expression builtInAsciiNumber(Expression in);
Expression builtInAsciiCharacter(Expression in);
