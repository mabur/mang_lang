#pragma once

struct Expression;

Expression builtInPutString(Expression rest, Expression top);
Expression builtInPutStack(Expression rest, Expression top);
Expression builtInPutStackValue(Expression rest, Expression top);

Expression builtInClear(Expression in);
Expression builtInClearTyped(Expression in);
Expression builtInPut(Expression in);
Expression builtInPutTyped(Expression in);
// The same, taking the item and the collection directly:
Expression builtInPut2(Expression item, Expression collection);
Expression builtInPutTyped2(Expression item, Expression collection);
Expression builtInTake(Expression in);
Expression builtInTakeTyped(Expression in);
Expression builtInDrop(Expression in);
Expression builtInDropTyped(Expression in);
Expression builtInGet(Expression in);
Expression builtInGetTyped(Expression in);
