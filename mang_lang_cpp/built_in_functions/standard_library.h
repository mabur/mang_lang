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

    get_index = in (index, list) out result@{
            i = 0,
            tail = list,
            while not equal(i, index),
                tail = rest tail,
                i = inc i,
            end,
            result = first tail
        },

    map = in (f, list) out result@{
            reversed_result = empty list,
            tail = list,
            while tail,
                reversed_result = prepend(f first tail, reversed_result),
                tail = rest tail,
            end,
            result = reverse reversed_result
        },

    filter = in (predicate, list) out result@{
            reversed_result = empty list,
            tail = list,
            while tail,
                reversed_result =
                    if predicate first tail then
                        prepend(first tail, reversed_result)
                    else
                        reversed_result,
                tail = rest tail,
            end,
            result = reverse reversed_result
        },

    count = in list out result@{
            result = 0,
            tail = list,
            while tail,
                result = inc result,
                tail = rest tail,
            end
        },

    count_item = in (item, list) out result@{
            result = 0,
            tail = list,
            while tail,
                result =
                    if equal(first tail, item) then
                        inc result
                    else
                        result,
                tail = rest tail,
            end
        },

    count_if = in (predicate, list) out result@{
            result = 0,
            tail = list,
            while tail,
                result =
                    if predicate first tail then
                        inc result
                    else
                        result,
                tail = rest tail,
            end
        },

    reverse = in list out result@{
            result = empty list,
            tail = list,
            while tail,
                result = prepend(first tail, result),
                tail = rest tail,
            end
        },

    range = in x out result@{
            result = (),
            y = x,
            while y,
                result = prepend(dec y, result),
                y = dec y,
            end
        },

    enumerate = in list out result@{
            reversed_result = (),
            tail = list,
            index = 0,
            while tail,
                reversed_result = prepend({index=index, item=first tail}, reversed_result),
                tail = rest tail,
                index = inc index,
            end,
            result = reverse reversed_result
        },

    concat = in (left, right) out result@{
            result = right,
            tail = reverse left,
            while tail,
                result = prepend(first tail, result),
                tail = rest tail,
            end
        }
}
)";
