#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Expression;
struct Number;

bool isEqual(const Number& number, const Expression* expression);
bool isEqual(const Character& character, const Expression* expression);

template<typename List>
bool isEqual(const List& list, const Expression* expression) {
    auto left = list.list();
    auto right = expression->list();
    for (; left && right; left = left->rest, right = right->rest) {
        if (!(left->first)->isEqual(right->first.get())) {
            return false;
        }
    }
    return !left && !right;
}
