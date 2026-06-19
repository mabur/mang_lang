#pragma once

#include "../expression.h"

Expression putString(Expression rest, Expression top);
Expression putStack(Expression rest, Expression top);
Expression putEvaluatedStack(Expression rest, Expression top);

namespace container_functions {

Expression clear(Expression in);
Expression clearTyped(Expression in);
Expression put(Expression in);
Expression putTyped(Expression in);
Expression take(Expression in);
Expression takeTyped(Expression in);
Expression drop(Expression in);
Expression dropTyped(Expression in);
Expression get(Expression in);
Expression getTyped(Expression in);

}
