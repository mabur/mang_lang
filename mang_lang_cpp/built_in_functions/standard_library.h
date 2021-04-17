#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = from x to add [x, 1],
    dec = from x to sub [x, 1],
    reverse_range = from x to
            if x then
                prepend {first = dec x, rest = reverse_range dec x}
            else
                [],
    range = from x to reverse reverse_range x,
    count = from list to
        if list then
            inc count rest list
        else
            0,
    count_item = from {list, item} to
        if list then
            result<{
                head = first list,
                tail = rest list,
                x = if equal [head, item] then 1 else 0,
                result = add [x, count_item {list=tail, item=item}],
            }
        else
            0,
    count_if = from {list, predicate} to
        if list then
            result<{
                head = first list,
                tail = rest list,
                x = if predicate head then 1 else 0,
                result = add [x, count_if {list=tail, predicate=predicate}],
            }
        else
            0,
    map = from {list, f} to
        if list then
            new_list<{
                new_first = f first list,
                new_rest = map {list = rest list, f = f},
                new_list = prepend {first = new_first, rest = new_rest}
            }
        else
            list,
    filter = from {list, predicate} to
        if list then
            new_list<{
                new_first = first list,
                new_rest = filter {list = rest list, predicate = predicate},
                new_list =
                    if predicate new_first then
                        prepend {first = new_first, rest = new_rest}
                    else
                        new_rest
            }
        else
            list,
    enumerate_from = from {list, index} to
        if list then
            result<{
                item = {index = index, item = first list},
                sub_result = enumerate_from{index=inc index, list=rest list},
                result = prepend {first = item, rest = sub_result}
            }
        else
            [],
    enumerate = from list to
        if list then
            enumerate_from {list = list, index = 0}
        else
            [],
    concat = from list to
        result<{
            left = first list,
            right = first rest list,
            result =
                if left then
                    prepend {first = first left, rest = concat [rest left, right]}
                else
                    right
        },
    get_index = from {list, index} to
        if equal[index, 0] then
            first list
        else
            get_index {index=dec index, list=rest list},
    second = from x to first rest x,
    third = from x to first rest rest x,
    fourth = from x to first rest rest rest x,
    fifth = from x to first rest rest rest rest x,
}
)";
