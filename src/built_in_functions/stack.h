#pragma once

struct Expression;

namespace stack_functions {

Expression clear(Expression in);
Expression clearTyped(Expression in);
Expression put(Expression in);
Expression putTyped(Expression in);
Expression take(Expression in);
Expression drop(Expression in);

}
