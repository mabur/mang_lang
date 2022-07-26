#pragma once

struct Expression;

namespace table_functions {

Expression get(Expression in);
Expression set(Expression in);
Expression putTable(Expression rest, Expression top);
Expression get_keys(Expression in);

}
