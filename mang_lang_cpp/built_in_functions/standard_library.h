#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = in x out add!(x 1)
    dec = in x out sub!(x 1)

    is_digit = in c out less_or_equal?(48 number!c 57)
    is_upper = in c out less_or_equal?(65 number!c 90)
    is_lower = in c out less_or_equal?(97 number!c 122)
    is_letter = in c out any?(is_upper?c is_lower?c)

    to_upper = in c out
        if is_lower?c then
            character!sub!(number!c 32)
        else
            c

    to_lower = in c out
        if is_upper?c then
            character!add!(number!c 32)
        else
            c

    range = in x out list@{
        list = ()
        x = x
        while x
            list = prepend!(dec!x list)
            x = dec!x
        end
    }

    drop = in (num_elements list) out short_list@{
        i = 0
        short_list = list
        while less?(i num_elements)
            i = inc!i
            short_list = rest@short_list
        end
    }

    take = in (num_elements list) out short_list@{
        reversed_result = empty!list
        i = 0
        list = list
        while less?(i num_elements)
            reversed_result = prepend!(first@list reversed_result)
            i = inc!i
            list = rest@list
        end
        short_list = reverse!reversed_result
    }

    get_index = in (index list) out first@drop!(index list)

    reverse = in list out reversed_list@{
        reversed_list = empty!list
        list = list
        while list
            reversed_list = prepend!(first@list reversed_list)
            list = rest@list
        end
    }

    concat = in (left_list right_list) out long_list@{
        long_list = right_list
        list = reverse!left_list
        while list
            long_list = prepend!(first@list long_list)
            list = rest@list
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
        reversed_result = ()
        list = list
        while list
            reversed_result = prepend!(f!first@list reversed_result)
            list = rest@list
        end
        result = reverse!reversed_result
    }

    map_string = in (f list) out result@{
        reversed_result = ""
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

    replace_if = in (predicate new_item list) out result@{
        reversed_result = empty!list
        list = list
        while list
            current_item = first@list
            item = if predicate?current_item then new_item else current_item
            reversed_result = prepend!(item reversed_result)
            list = rest@list
        end
        result = reverse!reversed_result
    }

    replace = in (old_item new_item list) out
        replace_if?(in x out equal?(x old_item) new_item list)

    count = in list out result@{
        result = 0
        list = list
        while list
            result = inc!result
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

    count_item = in (item list) out
        count_if!(in x out equal?(x item) list)

}
)";
