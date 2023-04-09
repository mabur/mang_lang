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

    newline = character!10

    is_digit = in c out is_increasing?[number!'0' number!c number!'9']
    is_upper = in c out is_increasing?[number!'A' number!c number!'Z']
    is_lower = in c out is_increasing?[number!'a' number!c number!'z']
    is_letter = in c out any?[is_upper?c is_lower?c]

    parse_digit = in c out sub!(number!c number!'0')

    parse_natural_number = in string out number@{
        string = reverse!string
        number = 0
        x = 1
        for c in string
            digit = parse_digit!c
            number = add!(number mul!(x digit))
            x = mul!(x 10)
        end
    }

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

    fold = in (operation container init) out result@{
        result = init
        for item in container
            result = operation!(item result)
        end
    }

    reverse = in container out fold!(
        put
        container
        clear!container
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

    map_stack = in (f container) out reverse!fold!(
        in (item container) out put!(f!item container)
        container
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

    map = in (f container) out reverse!fold!(
        in (item container) out put!(f!item container)
        container
        clear!container
    )

    zip2 = in (a b) out reverse!result@{
        result = []
        while and?[a b]
            result += (take!a take!b)
            a--
            b--
        end
    }

    zip3 = in (a b c) out reverse!result@{
        result = []
        while and?[a b c]
            result += (take!a take!b take!c)
            a--
            b--
            c--
        end
    }

    zip4 = in (a b c d) out reverse!result@{
        result = []
        while and?[a b c d]
            result += (take!a take!b take!c take!d)
            a--
            b--
            c--
            d--
        end
    }

    consecutive_pairs = in container out reverse!result@{
        result = []
        while if container then boolean!drop!container else no
            result += (take!container take!drop!container)
            container--
        end
    }

    min = in (left right) out if less?(left right) then left else right
    max = in (left right) out if less?(left right) then right else left

    min_item = in container out fold!(min container inf)

    max_item = in container out fold!(max container -inf)

    min_predicate = in (predicate container) out fold!(
        in (left right) out if predicate?(left right) then left else right
        drop!container
        take!container
    )

    max_predicate = in (predicate container) out fold!(
        in (left right) out if predicate?(left right) then right else left
        drop!container
        take!container
    )

    min_key = in (key container) out fold!(
        in (left right) out if less?(key!left key!right) then left else right
        drop!container
        take!container
    )

    max_key = in (key container) out fold!(
        in (left right) out if less?(key!left key!right) then right else left
        drop!container
        take!container
    )

    sum = in container out fold!(add container 0)

    product = in container out fold!(mul container 1)

    clear_if = in (predicate container) out reverse!fold!(
        in (item container) out
            if predicate?item then
                container
            else
                put!(item container)
        container
        clear!container
    )

    clear_item = in (item container) out
        clear_if?(in x out equal?(x item) container)

    take_many = in (n container) out reverse!stack_out@{
        stack_out = clear!container
        for _ in n
            stack_out += take!container
            container--
        end
    }

    take_while = in (predicate container) out reverse!stack_out@{
        stack_out = clear!container
        while if container then predicate?take!container else no
            stack_out += take!container
            container--
        end
    }

    take_until_item = in (item container) out
        take_while!(in x out unequal?(x item) container)

    drop_many = in (n container) out container@{
        container = container
        for _ in n
            container--
        end
    }

    drop_while = in (predicate container) out container@{
        container = container
        while if container then predicate?take!container else no
            container--
        end
    }

    drop_until_item = in (item container) out
        drop_while?(in x out unequal?(x item) container)

    replace = in (new_item container) out fold!(
        in (item container) out put!(new_item container)
        container
        clear!container
    )

    replace_if = in (predicate new_item container) out reverse!fold!(
        in (item container) out
            if predicate?item then
                put!(new_item container)
            else
                put!(item container)
        container
        clear!container
    )

    replace_item = in (old_item new_item container) out
        replace_if?(in x out equal?(x old_item) new_item container)

    count = in container out fold!(
        in (item n) out inc!n
        container
        0
    )

    count_if = in (predicate container) out fold!(
        in (item n) out if predicate?item then inc!n else n
        container
        0
    )

    count_item = in (item container) out
        count_if!(in x out equal?(x item) container)

    range = in n out container@{
        container = []
        while n
            n--
            container += n
        end
    }

    enumerate = in container out zip2!(range!count!container container)

    get0 = in container out container!0
    get1 = in container out container!1
    get2 = in container out container!2
    get3 = in container out container!3
    get4 = in container out container!4
    get5 = in container out container!5
    get6 = in container out container!6
    get7 = in container out container!7
    get8 = in container out container!8
    get9 = in container out container!9

    split = in (delimiter container) out reverse!result@{
        word = take_until_item!(delimiter container)
        container = drop_until_item!(delimiter container)
        result = [word]
        while container
            container--
            word = take_until_item!(delimiter container)
            container = drop_until_item!(delimiter container)
            result += word
        end
    }

    cartesian_product2 = in (a b) out result@{
        result = []
        for item_b in b
            c = a
            for item_c in c
                result += (item_c item_b)
            end
        end
    }

    put_column = in (column rows) out reverse!new_rows@{
        new_rows = []
        for item in column
            row = take!rows
            row += item
            new_rows += row
            rows--
        end
    }

    transpose = in rows out map!(reverse columns@{
        columns = replace!([] take!rows)
        for row in rows
            columns = put_column!(row columns)
        end
    })

    all = in container out not?drop_while!(boolean container)
    none = in container out not?drop_while!(not container)
    any = in container out boolean?drop_while!(not container)

    and = in container out all?container
    or = in container out any?container

    is_increasing = in container out not?drop_while!(
        less_or_equal
        consecutive_pairs!container
    )

    less_or_equal_top = in (left right) out
        if left then
            if right then
                is_increasing?[take!left take!right]
            else
                yes
        else
            no

    merge_sorted = in (left right) out reverse!container@{
        container = []
        while or?[left right]
            while less_or_equal_top?(left right)
                container += take!left
                left--
            end
            while less_or_equal_top?(right left)
                container += take!right
                right--
            end
        end
    }

    unique = in container out get_keys!fold!(
        in (item table) out put!((item 0) table)
        container
        <>
    )

    count_elements = in container out fold!(
        in (item table) out put!((item inc!get!(item table 0)) table)
        container
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

    addv = in (a b) out map!(add zip2!(a b))
    subv = in (a b) out map!(sub zip2!(a b))
    mulv = in (a b) out map!(mul zip2!(a b))
    divv = in (a b) out map!(div zip2!(a b))
    dot = in (a b) out sum!mulv!(a b)
    squared_norm = in a out dot!(a a)
    norm = in a out sqrt!squared_norm!a
}
)";
