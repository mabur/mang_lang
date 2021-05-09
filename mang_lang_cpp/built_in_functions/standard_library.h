#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = from x to add [x, 1],
    dec = from x to sub [x, 1],
    reverse_range = from x to
        if x then
            prepend [dec x, reverse_range dec x]
        else
            [],
    range = from x to reverse reverse_range x,
    count = from list to
        if list then
            inc count rest list
        else
            0,
    count_item = from [item, list] to
        if list then
            if equal [first list, item] then
                inc count_item [item, rest list]
            else
                count_item [item, rest list]
        else
            0,
    count_if = from [predicate, list] to
        if list then
            if predicate first list then
                inc count_if [predicate, rest list]
            else
                count_if [predicate, rest list]
        else
            0,
    map = from [f, list] to
        if list then
            prepend [f first list, map [f, rest list]]
        else
            list,
    filter = from [predicate, list] to
        if list then
            if predicate first list then
                prepend [first list, filter [predicate, rest list]]
            else
                filter [predicate, rest list]
        else
            list,
    enumerate_from = from {list, index} to
        if list then
            result@{
                item = {index = index, item = first list},
                sub_result = enumerate_from{index=inc index, list=rest list},
                result = prepend [item, sub_result]
            }
        else
            [],
    enumerate = from list to
        if list then
            enumerate_from {list = list, index = 0}
        else
            [],
    concat = from list to
        result@{
            left = first list,
            right = first rest list,
            result =
                if left then
                    prepend [first left, concat [rest left, right]]
                else
                    right
        },
    get_index = from [index, list] to
        if index then
            get_index [dec index, rest list]
        else
            first list,
    second = from x to first rest x,
    third = from x to first rest rest x,
    fourth = from x to first rest rest rest x,
    fifth = from x to first rest rest rest rest x,
}
)";
