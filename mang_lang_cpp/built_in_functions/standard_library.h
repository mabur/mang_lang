#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    boolean = in x out if x then yes else no
    not = in x out if x then no else yes

    equal = in (left right) out is left right then yes else no
    unequal = in (left right) out is left right then no else yes

    less_or_equal = in (left right) out not?less?(right left)

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

    first = in stack out take!stack
    second = in stack out take!drop!stack
    third = in stack out take!drop!drop!stack
    fourth = in stack out take!drop!drop!drop!stack
    fifth = in stack out take!drop!drop!drop!drop!stack
    sixth = in stack out take!drop!drop!drop!drop!drop!stack
    seventh = in stack out take!drop!drop!drop!drop!drop!drop!stack
    eighth = in stack out take!drop!drop!drop!drop!drop!drop!drop!stack
    ninth = in stack out take!drop!drop!drop!drop!drop!drop!drop!drop!stack
    tenth = in stack out take!drop!drop!drop!drop!drop!drop!drop!drop!drop!stack

    fold = in (operation stack init) out result@{
        result = init
        stack = stack
        while stack
            result = operation!(take!stack result)
            stack = drop!stack
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

    make_stack = in container out reverse!put_each!(
        container
        []
    )

    make_string = in container out reverse!put_each!(
        container
        ""
    )

    make_table = in container out put_each!(
        container
        <>
    )

    merge_stack = in stacks out reverse!fold!(
        put_each
        stacks
        []
    )

    merge_string = in strings out reverse!fold!(
        put_each
        strings
        ""
    )

    merge_table = in tables out fold!(
        put_each
        reverse!tables
        <>
    )

    map_stack = in (f stack) out reverse!fold!(
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
            result = put!((take!left take!right) result)
            left = drop!left
            right = drop!right
        end
    }

    consecutive_pairs = in stack out reverse!result@{
        result = []
        while if stack then boolean!drop!stack else no
            result = put!((take!stack take!drop!stack) result)
            stack = drop!stack
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

    drop_many = in (n stack) out stack@{
        n = n
        stack = stack
        while n
            n = dec!n
            stack = drop!stack
        end
    }

    drop_while = in (predicate stack) out stack@{
        stack = stack
        while if stack then predicate?take!stack else no
            stack = drop!stack
        end
    }

    drop_until_item = in (item stack) out
        drop_while?(in x out unequal?(x item) stack)

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

    take_many = in (n stack) out reverse!stack_out@{
        stack_out = clear!stack
        stack = stack
        n = n
        while n
            n = dec!n
            stack_out = put!(take!stack stack_out)
            stack = drop!stack
        end
    }

    take_while = in (predicate stack) out reverse!stack_out@{
        stack_out = clear!stack
        stack = stack
        while if stack then predicate?take!stack else no
            stack_out = put!(take!stack stack_out)
            stack = drop!stack
        end
    }

    take_until_item = in (item stack) out
        take_while!(in x out unequal?(x item) stack)

    split = in (n stack) out stacks@{
        top_stack = clear!stack
        bottom_stack = stack
        while and?[n bottom_stack]
            n = dec!n
            top_stack = put!(take!bottom_stack top_stack)
            bottom_stack = drop!bottom_stack
        end
        stacks = [top_stack bottom_stack]
    }

    get_index = in (index stack) out take!drop_many!(index stack)

    all = in stack out not?drop_while!(boolean stack)
    none = in stack out not?drop_while!(not stack)
    any = in stack out boolean?drop_while!(not stack)

    is_increasing = in stack out not?drop_while!(
        less_or_equal
        consecutive_pairs!stack
    )

    and = in stack out all?stack
    or = in stack out any?stack

    less_or_equal_top = in (left right) out
        if left then
            if right then
                is_increasing?[take!left take!right]
            else
                yes
        else
            no

    merge_sorted = in (left right) out reverse!stack@{
        stack = []
        while or?[left right]
            while less_or_equal_top?(left right)
                stack = put!(take!left stack)
                left = drop!left
            end
            while less_or_equal_top?(right left)
                stack = put!(take!right stack)
                right = drop!right
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

    get_items = make_stack

    get_keys = in table out map_stack!(
        in (key value) out key
        table
    )

    get_values = in table out map_stack!(
        in (key value) out value
        table
    )
}
)";
