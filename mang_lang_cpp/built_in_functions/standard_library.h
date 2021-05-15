#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = in x out add(x, 1),

    dec = in x out sub(x, 1),

    second = in x out first rest x,

    third = in x out first rest rest x,

    fourth = in x out first rest rest rest x,

    fifth = in x out first rest rest rest rest x,

    get_index = in (index, list) out
        if index then
            get_index(dec index, rest list)
        else
            first list,

    map = in (f, list) out
        if list then
            prepend(f first list, map(f, rest list))
        else
            list,

    filter = in (predicate, list) out
        if list then
            if predicate first list then
                prepend(first list, filter(predicate, rest list))
            else
                filter(predicate, rest list)
        else
            list,

    count = in list out
        if list then
            inc count rest list
        else
            0,

    count_item = in (item, list) out
        if list then
            if equal(first list, item) then
                inc count_item(item, rest list)
            else
                count_item(item, rest list)
        else
            0,

    count_if = in (predicate, list) out
        if list then
            if predicate first list then
                inc count_if(predicate, rest list)
            else
                count_if(predicate, rest list)
        else
            0,

    reverse_range = in x out
        if x then
            prepend(dec x, reverse_range dec x)
        else
            (),

    range = in x out reverse reverse_range x,

    enumerate_from = in (index, list) out
        if list then
            result@{
                item = {index = index, item = first list},
                sub_result = enumerate_from(inc index, rest list),
                result = prepend(item, sub_result)
            }
        else
            (),

    enumerate = in list out
        if list then
            enumerate_from(0, list)
        else
            (),

    concat = in (left, right) out
        if left then
            prepend(first left, concat(rest left, right))
        else
            right
}
)";
