#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = in x out add!(x 1)

    dec = in x out sub!(x 1)

    drop = in (index list) out list@{
        i = 0
        list = list
        while less?(i index)
            i = inc!i
            list = rest@list
        end
    }

    get_index = in (index list) out first@drop!(index list)

    reverse = in list out result@{
        result = empty!list
        list = list
        while list
            result = prepend!(first@list result)
            list = rest@list
        end
    }

    concat = in (left right) out result@{
        result = right
        list = reverse!left
        while list
            result = prepend!(first@list result)
            list = rest@list
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
        list = list
        index = 0
        while list
            reversed_result = prepend!(
                {index=index item=first@list}
                reversed_result
            )
            index = inc!index
            list = rest@list
        end
        result = reverse!reversed_result
    }

    map = in (f list) out result@{
        reversed_result = empty!list
        list = list
        while list
            reversed_result = prepend!(f!first@list reversed_result)
            list = rest@list
        end
        result = reverse!reversed_result
    }

    filter = in (predicate list) out result@{
        reversed_result = empty!list
        list = list
        while list
            reversed_result =
                if predicate?first@list then
                    prepend!(first@list reversed_result)
                else
                    reversed_result
            list = rest@list
        end
        result = reverse!reversed_result
    }

    replace = in (old_item new_item list) out
        map!(in item out if equal?(item old_item) then new_item else item list)

    count = in list out result@{
        result = 0
        list = list
        while list
            result = inc!result
            list = rest@list
        end
    }

    count_item = in (item list) out result@{
        result = 0
        list = list
        while list
            result =
                if equal?(first@list item) then
                    inc!result
                else
                    result
            list = rest@list
        end
    }

    count_if = in (predicate list) out result@{
        result = 0
        list = list
        while list
            result =
                if predicate?first@list then
                    inc!result
                else
                    result
            list = rest@list
        end
    }
}
)";
