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

    fold = in (operation stack init) out result@{
        result = init
        stack = stack
        while stack
            result = operation!(top@stack result)
            stack = rest@stack
        end
    }

    min = in stack out fold!(
        in (item value) out if less?(item value) then item else value
        stack
        inf
    )

    max = in stack out fold!(
        in (item value) out if less?(item value) then value else item
        stack
        -inf
    )

    put_each = in (top_stack bottom_stack) out fold!(
        put
        top_stack
        bottom_stack
    )

    reverse = in stack out
        put_each!(stack clear!stack)

    concat = in stacks out reverse!fold!(
        put_each
        stacks
        ()
    )

    concat_strings = in strings out reverse!fold!(
        put_each
        strings
        ""
    )

    map = in (f stack) out reverse!fold!(
        in (item stack) out put!(f!item stack)
        stack
        ()
    )

    map_string = in (f stack) out reverse!fold!(
        in (item stack) out put!(f!item stack)
        stack
        ""
    )

    clear_if = in (predicate stack) out reverse!fold!(
        in (item stack) out
            if predicate?item then
                stack
            else
                put!(item stack)
        stack
        clear!stack
    )

    clear_item = in (item stack) out
        clear_if?(in x out equal?(x item) stack)

    replace = in (new_item stack) out fold!(
        in (item stack) out put!(new_item stack)
        stack
        clear!stack
    )

    replace_if = in (predicate new_item stack) out reverse!fold!(
        in (item stack) out
            if predicate?item then
                put!(new_item stack)
            else
                put!(item stack)
        stack
        clear!stack
    )

    replace_item = in (old_item new_item stack) out
        replace_if?(in x out equal?(x old_item) new_item stack)

    count = in stack out fold!(
        in (item n) out inc!n
        stack
        0
    )

    count_if = in (predicate stack) out fold!(
        in (item n) out if predicate?item then inc!n else n
        stack
        0
    )

    count_item = in (item stack) out
        count_if!(in x out equal?(x item) stack)

    range = in n out stack@{
        stack = ()
        i = n
        while i
            i = dec!i
            stack = put!(i stack)
        end
    }

    enumerate = in stack out result@{
        reversed_result = ()
        stack = stack
        index = 0
        while stack
            reversed_result = put!(
                {index=index item=top@stack}
                reversed_result
            )
            index = inc!index
            stack = rest@stack
        end
        result = reverse!reversed_result
    }

    split = in (n stack) out stacks@{
        top_stack = clear!stack
        bottom_stack = stack
        while and?(n bottom_stack)
            n = dec!n
            top_stack = put!(top@bottom_stack top_stack)
            bottom_stack = rest@bottom_stack
        end
        stacks = (top_stack bottom_stack)
    }

    drop = in (n stack) out short_stack@{
        short_stack = stack
        i = n
        while i
            i = dec!i
            short_stack = rest@short_stack
        end
    }

    drop_while = in (predicate stack) out stack@{
        stack = stack
        while if stack then predicate?top@stack else no
            stack = rest@stack
        end
    }

    take = in (n stack) out short_stack@{
        reversed_result = clear!stack
        stack = stack
        i = n
        while i
            i = dec!i
            reversed_result = put!(top@stack reversed_result)
            stack = rest@stack
        end
        short_stack = reverse!reversed_result
    }

    get_index = in (index stack) out top@drop!(index stack)

    all = in stack out not?drop_while!(boolean stack)
    none = in stack out not?drop_while!(not stack)
    any = in stack out boolean?drop_while!(not stack)
}
)";
