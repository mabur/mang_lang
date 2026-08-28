#pragma once

struct Expression;

Expression builtInPutString(Expression rest, Expression top);
Expression builtInPutStack(Expression rest, Expression top);
Expression builtInPutEvaluatedStack(Expression rest, Expression top);

Expression builtInClear(Expression in);
Expression builtInClearTyped(Expression in);
Expression builtInPut(Expression in);
Expression builtInPutTyped(Expression in);
Expression builtInTake(Expression in);
Expression builtInTakeTyped(Expression in);
Expression builtInDrop(Expression in);
Expression builtInDropTyped(Expression in);
Expression builtInGet(Expression in);
Expression builtInGetTyped(Expression in);
