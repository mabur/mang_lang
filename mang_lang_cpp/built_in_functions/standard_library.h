#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    boolean = in x out if x then yes else no
    not = in x out if x then no else yes
    and = in (left right) out if left then boolean?right else no
    or = in (left right) out if left then yes else boolean?right
    unequal = in pair out not?equal?pair

    pi = 3.14159265359
    tau = 6.28318530718

    inc = in x out add!(x 1)
    dec = in x out sub!(x 1)
    neg = in x out sub!(0 x)
    abs = in x out if less?(0 x) then x else neg!x

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

    fold = in (operation list init) out result@{
        result = init
        list = list
        while list
            result = operation!(first@list result)
            list = rest@list
        end
    }

    min = in list out fold!(
        in (item value) out if less?(item value) then item else value
        list
        inf
    )

    max = in list out fold!(
        in (item value) out if less?(item value) then value else item
        list
        -inf
    )

    reverse = in list out fold!(
        in (item list) out prepend!(item list)
        list
        clear!list
    )

    concat = in (left_list right_list) out fold!(
        in (item list) out prepend!(item list)
        reverse!left_list
        right_list
    )

    map = in (f list) out reverse!fold!(
        in (item list) out prepend!(f!item list)
        list
        ()
    )

    map_string = in (f list) out reverse!fold!(
        in (item list) out prepend!(f!item list)
        list
        ""
    )

    clear_if = in (predicate list) out reverse!fold!(
        in (item list) out
            if predicate?item then
                list
            else
                prepend!(item list)
        list
        clear!list
    )

    clear_item = in (item list) out
        clear_if?(in x out equal?(x item) list)

    replace = in (new_item list) out fold!(
        in (item list) out prepend!(new_item list)
        list
        clear!list
    )

    replace_if = in (predicate new_item list) out reverse!fold!(
        in (item list) out
            if predicate?item then
                prepend!(new_item list)
            else
                prepend!(item list)
        list
        clear!list
    )

    replace_item = in (old_item new_item list) out
        replace_if?(in x out equal?(x old_item) new_item list)

    count = in list out fold!(
        in (item n) out inc!n
        list
        0
    )

    count_if = in (predicate list) out fold!(
        in (item n) out if predicate?item then inc!n else n
        list
        0
    )

    count_item = in (item list) out
        count_if!(in x out equal?(x item) list)

    range = in n out list@{
        list = ()
        i = n
        while i
            i = dec!i
            list = prepend!(i list)
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

    drop = in (n list) out short_list@{
        short_list = list
        i = n
        while i
            i = dec!i
            short_list = rest@short_list
        end
    }

    drop_while = in (predicate list) out list@{
        list = list
        while if list then predicate?first@list else no
            list = rest@list
        end
    }

    take = in (n list) out short_list@{
        reversed_result = clear!list
        list = list
        i = n
        while i
            i = dec!i
            reversed_result = prepend!(first@list reversed_result)
            list = rest@list
        end
        short_list = reverse!reversed_result
    }

    get_index = in (index list) out first@drop!(index list)

    all = in list out not?drop_while!(boolean list)
    none = in list out not?drop_while!(not list)
    any = in list out boolean?drop_while!(not list)
}
)";
