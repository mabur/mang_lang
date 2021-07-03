#pragma once

struct Dictionary;
struct Number;

bool boolean(const Dictionary& dictionary);
bool boolean(const Number& number);

template <typename List>
bool boolean(const List& list) {
    return !!list.list();
}
