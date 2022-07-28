#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    boolean = in x out if x then yes else no
    not = in x out if x then no else yes

    equal = in (left right) out is left right then yes else no
    unequal = in (left right) out is left right then no else yes

    greater = in (left right) out less?(right left)

    inf = div!(1 0)
    nan = div!(0 0)
    pi = 3.14159265359
    tau = 6.28318530718

    inc = in x out add!(x 1)
    dec = in x out sub!(x 1)
    neg = in x out sub!(0 x)
    abs = in x out if less?(0 x) then x else neg!x

    is_upper = in c out is_increasing?[65 number!c 90]
    is_lower = in c out is_increasing?[97 number!c 122]
    is_letter = in c out any?[is_upper?c is_lower?c]
    is_digit = in c out is_increasing?[48 number!c 57]

    parse_digit = in c out sub!(number!c 48)

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

    first = in stack out top@stack
    second = in stack out top@rest@stack
    third = in stack out top@rest@rest@stack
    fourth = in stack out top@rest@rest@rest@stack
    fifth = in stack out top@rest@rest@rest@rest@stack
    sixth = in stack out top@rest@rest@rest@rest@rest@stack
    seventh = in stack out top@rest@rest@rest@rest@rest@rest@stack
    eighth = in stack out top@rest@rest@rest@rest@rest@rest@rest@stack
    ninth = in stack out top@rest@rest@rest@rest@rest@rest@rest@rest@stack
    tenth = in stack out top@rest@rest@rest@rest@rest@rest@rest@rest@rest@stack

    fold = in (operation stack init) out result@{
        result = init
        stack = stack
        while stack
            result = operation!(top@stack result)
            stack = rest@stack
        end
    }

    sum = in stack out fold!(
        add
        stack
        0
    )

    product = in stack out fold!(
        mul
        stack
        1
    )

    reverse = in stack out fold!(
        put
        stack
        clear!stack
    )

    put_each = in (top_stack bottom_stack) out fold!(
        put
        top_stack
        bottom_stack
    )

    copy_stack = in container out reverse!put_each!(
        container
        []
    )

    copy_string = in container out reverse!put_each!(
        container
        ""
    )

    copy_table = in container out put_each!(
        container
        <>
    )

    concat = in stacks out reverse!fold!(
        put_each
        stacks
        []
    )

    concat_strings = in strings out reverse!fold!(
        put_each
        strings
        ""
    )

    concat_tables = in tables out fold!(
        put_each
        reverse!tables
        <>
    )

    map = in (f stack) out reverse!fold!(
        in (item stack) out put!(f!item stack)
        stack
        []
    )

    map_string = in (f string) out reverse!fold!(
        in (item string) out put!(f!item string)
        string
        ""
    )

    map_table = in (f table) out fold!(
        in (item table) out put!(f!item table)
        table
        <>
    )

    zip = in (left right) out reverse!result@{
        result = []
        while and?[left right]
            result = put!((top@left top@right) result)
            left = rest@left
            right = rest@right
        end
    }

    consecutive_pairs = in stack out reverse!result@{
        result = []
        while if stack then boolean!rest@stack else no
            result = put!((top@stack top@rest@stack) result)
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

    find_if = in (predicate stack) out stack@{
        stack = stack
        while if stack then not!predicate?top@stack else no
            stack = rest@stack
        end
    }

    find_item = in (item stack) out
        find_if?(in x out equal?(x item) stack)

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
        stack = []
        i = n
        while i
            i = dec!i
            stack = put!(i stack)
        end
    }

    enumerate = in stack out zip!(range!count!stack stack)

    drop = in (n stack) out short_stack@{
        short_stack = stack
        i = n
        while i
            i = dec!i
            short_stack = rest@short_stack
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

    split = in (n stack) out stacks@{
        top_stack = clear!stack
        bottom_stack = stack
        while and?[n bottom_stack]
            n = dec!n
            top_stack = put!(top@bottom_stack top_stack)
            bottom_stack = rest@bottom_stack
        end
        stacks = [top_stack bottom_stack]
    }

    get_index = in (index stack) out top@drop!(index stack)

    all = in stack out not?find_if!(not stack)
    none = in stack out not?find_if!(boolean stack)
    any = in stack out boolean?find_if!(boolean stack)

    is_increasing = in stack out not?find_if!(greater consecutive_pairs!stack)

    and = in stack out all?stack
    or = in stack out any?stack

    less_or_equal_top = in (left right) out
        if left then
            if right then
                is_increasing?[top@left top@right]
            else
                yes
        else
            no

    merge_sorted = in (left right) out reverse!stack@{
        stack = []
        while or?[left right]
            while less_or_equal_top?(left right)
                stack = put!(top@left stack)
                left = rest@left
            end
            while less_or_equal_top?(right left)
                stack = put!(top@right stack)
                right = rest@right
            end
        end
    }

    unique = in stack out get_keys!fold!(
        in (item table) out put!((item 0) table)
        stack
        <>
    )

    count_elements = in stack out fold!(
        in (item table) out put!((item inc!get!(item table 0)) table)
        stack
        <>
    )

    get_items = copy_stack

    get_keys = in table out map!(
        in (key value) out key
        table
    )

    get_values = in table out map!(
        in (key value) out value
        table
    )
}
)";
