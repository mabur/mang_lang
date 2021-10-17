#include "logic.h"

#include "../factory.h"
#include "../container.h"

namespace logic {

bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left).value == getNumber(right).value;
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left).value == getCharacter(right).value;
    }
    if (left_type == BOOLEAN && right_type == BOOLEAN) {
        return getBoolean(left).value == getBoolean(right).value;
    }
    if (left_type == EMPTY_LIST && right_type == EMPTY_LIST) {
        return true;
    }
    if (left_type == LIST && right_type == LIST) {
        return new_list::allOfPairs(left, right, isEqual);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return new_string::allOfPairs(left, right, isEqual);
    }
    return false;
}

Expression equal(Expression in) {
    const auto binary = new_list::getBinaryInput(in);
    const auto result = isEqual(binary.left, binary.right);
    return makeBoolean(new Boolean{CodeRange{}, result});
}

}
