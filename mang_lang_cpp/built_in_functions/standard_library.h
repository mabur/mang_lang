#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = from x to add(x, 1),

    dec = from x to sub(x, 1),

    second = from x to first rest x,

    third = from x to first rest rest x,

    fourth = from x to first rest rest rest x,

    fifth = from x to first rest rest rest rest x,

    get_index = from (index, list) to
        if index then
            get_index(dec index, rest list)
        else
            first list,

    map = from (f, list) to
        if list then
            prepend(f first list, map(f, rest list))
        else
            list,

    filter = from (predicate, list) to
        if list then
            if predicate first list then
                prepend(first list, filter(predicate, rest list))
            else
                filter(predicate, rest list)
        else
            list,

    count = from list to
        if list then
            inc count rest list
        else
            0,

    count_item = from (item, list) to
        if list then
            if equal(first list, item) then
                inc count_item(item, rest list)
            else
                count_item(item, rest list)
        else
            0,

    count_if = from (predicate, list) to
        if list then
            if predicate first list then
                inc count_if(predicate, rest list)
            else
                count_if(predicate, rest list)
        else
            0,

    reverse_range = from x to
        if x then
            prepend(dec x, reverse_range dec x)
        else
            (),

    range = from x to reverse reverse_range x,

    enumerate_from = from (index, list) to
        if list then
            result@{
                item = {index = index, item = first list},
                sub_result = enumerate_from(inc index, rest list),
                result = prepend(item, sub_result)
            }
        else
            (),

    enumerate = from list to
        if list then
            enumerate_from(0, list)
        else
            (),

    concat = from (left, right) to
        if left then
            prepend(first left, concat(rest left, right))
        else
            right
}
)";
