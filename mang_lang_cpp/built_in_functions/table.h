#pragma once

struct Expression;

namespace table_functions {

Expression get(Expression in);

Expression set(Expression in);

Expression get_keys(Expression in);

}
