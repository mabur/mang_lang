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
    count_item = from input to
        if list<input then
            result<{
                item = item<input,
                list = list<input,
                head = first list,
                tail = rest list,
                x = if equal [head, item] then 1 else 0,
                result = add [x, count_item {list=tail, item=item}],
            }
        else
            0,
    map = from input to
        if list<input then
            new_list<{
                list = list<input,
                f = f<input,
                new_first = f first list,
                new_rest = map {list = rest list, f = f},
                new_list = prepend {first = new_first, rest = new_rest}
            }
        else
            list<input,
    filter = from input to
        if list<input then
            new_list<{
                list = list<input,
                predicate = predicate<input,
                new_first = first list,
                new_rest = filter {list = rest list, predicate = predicate},
                new_list =
                    if predicate new_first then
                        prepend {first = new_first, rest = new_rest}
                    else
                        new_rest
            }
        else
            list<input,
    get_index = from input to
        if equal[index<input, 0] then
            first list<input
        else
            get_index {index=dec index<input, list=rest list<input}
}
)";
