#pragma once

struct Expression;

namespace table_functions {

Expression get(Expression in);
Expression put(Expression rest, Expression top);
Expression putTyped(Expression rest, Expression top);

}
