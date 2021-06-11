#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = in x out add!(x 1)

    dec = in x out sub!(x 1)

    second = in x out first@rest@x

    third = in x out first@rest@rest@x

    fourth = in x out first@rest@rest@rest@x

    fifth = in x out first@rest@rest@rest@rest@x

    drop = in (index list) out tail@{
        i = 0
        tail = list
        while less!(i index)
            tail = rest@tail
            i = inc!i
        end
    }

    get_index = in (index list) out first@drop!(index list)

    reverse = in list out result@{
        result = empty!list
        tail = list
        while tail
            result = prepend!(first@tail result)
            tail = rest@tail
        end
    }

    concat = in (left right) out result@{
        result = right
        tail = reverse!left
        while tail
            result = prepend!(first@tail result)
            tail = rest@tail
        end
    }

    range = in x out result@{
        result = ()
        y = x
        while y
            result = prepend!(dec!y result)
            y = dec!y
        end
    }

    enumerate = in list out result@{
        reversed_result = ()
        tail = list
        index = 0
        while tail
            reversed_result = prepend!(
                {index=index item=first@tail}
                reversed_result
            )
            tail = rest@tail
            index = inc!index
        end
        result = reverse!reversed_result
    }

    map = in (f list) out result@{
        reversed_result = empty!list
        tail = list
        while tail
            reversed_result = prepend!(f!first@tail reversed_result)
            tail = rest@tail
        end
        result = reverse!reversed_result
    }

    filter = in (predicate list) out result@{
        reversed_result = empty!list
        tail = list
        while tail
            reversed_result =
                if predicate!first@tail then
                    prepend!(first@tail reversed_result)
                else
                    reversed_result
            tail = rest@tail
        end
        result = reverse!reversed_result
    }

    replace = in (old_item new_item list) out
        map!(in item out if equal!(item old_item) then new_item else item list)

    count = in list out result@{
        result = 0
        tail = list
        while tail
            result = inc!result
            tail = rest@tail
        end
    }

    count_item = in (item list) out result@{
        result = 0
        tail = list
        while tail
            result =
                if equal!(first@tail item) then
                    inc!result
                else
                    result
            tail = rest@tail
        end
    }

    count_if = in (predicate list) out result@{
        result = 0
        tail = list
        while tail
            result =
                if predicate!first@tail then
                    inc!result
                else
                    result
            tail = rest@tail
        end
    }
}
)";
