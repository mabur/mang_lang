#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = from x to add [x, 1],
    dec = from x to sub [x, 1],
    count = from list to
        if list then
            inc count rest list
        else
            0,
    map = from input to
        if list<input then
            new_list<{
                list = list<input,
                f = f<input,
                new_first = f first list,
                new_rest = map {list = rest list, f = f},
                new_list = prepend {item = new_first, list = new_rest}
            }
        else
            list<input
}
)";
