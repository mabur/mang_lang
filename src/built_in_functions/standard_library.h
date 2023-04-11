#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    Number = 0
    Boolean = no
    Character = 'a'
    Stack = []
    String = ""
    Table = <>
    Numbers = [Number]
    Function = in x out x

    boolean = in x out Boolean:if x then yes else no
    not = in x out Boolean:if x then no else yes

    equal = in (left right) out Boolean:is left right then yes else no
    unequal = in (left right) out Boolean:is left right then no else yes

    less_or_equal = in (Number:left Number:right) out Boolean:not?less?(right left)

    inf = div!(1 0)
    nan = div!(0 0)
    pi = 3.14159265359
    tau = 6.28318530718

    inc = in Number:x out Number:add!(x 1)
    dec = in Number:x out Number:sub!(x 1)
    neg = in Number:x out Number:sub!(0 x)
    abs = in Number:x out Number:if less?(0 x) then x else neg!x

    newline = character!10

    is_digit = in Character:c out Boolean:is_increasing?[number!'0' number!c number!'9']
    is_upper = in Character:c out Boolean:is_increasing?[number!'A' number!c number!'Z']
    is_lower = in Character:c out Boolean:is_increasing?[number!'a' number!c number!'z']
    is_letter = in Character:c out Boolean:any?[is_upper?c is_lower?c]

    parse_digit = in Character:c out Number:sub!(number!c number!'0')

    parse_natural_number = in String:string out Number:number@{
        string = reverse!string
        number = 0
        x = 1
        for c in string
            digit = parse_digit!c
            number = add!(number mul!(x digit))
            x = mul!(x 10)
        end
    }

    to_upper = in Character:c out Character :
        if is_lower?c then
            character!sub!(number!c 32)
        else
            c

    to_lower = in Character:c out Character :
        if is_upper?c then
            character!add!(number!c 32)
        else
            c

    fold = in (Function:operation container init) out result@{
        result = init
        for item in container
            result = operation!(item result)
        end
    }

    reverse = in container out container:fold!(
        put
        container
        clear!container
    )

    put_each = in (top_stack bottom_stack) out bottom_stack:fold!(
        put
        top_stack
        bottom_stack
    )

    make_stack = in container out Stack:reverse!put_each!(
        container
        []
    )

    make_string = in container out String:reverse!put_each!(
        container
        ""
    )

    make_table = in container out Table:put_each!(
        container
        <>
    )

    merge_stack = in stacks out Stack:reverse!fold!(
        put_each
        stacks
        []
    )

    merge_string = in strings out String:reverse!fold!(
        put_each
        strings
        ""
    )

    merge_table = in tables out Table:fold!(
        put_each
        reverse!tables
        <>
    )

    map_stack = in (Function:f container) out Stack:reverse!fold!(
        in (item container) out put!(f!item container)
        container
        []
    )

    map_string = in (Function:f container) out String:reverse!fold!(
        in (item container) out put!(f!item container)
        container
        ""
    )

    map_table = in (Function:f container) out Table:fold!(
        in (item container) out put!(f!item container)
        container
        <>
    )

    map = in (Function:f container) out container:reverse!fold!(
        in (item container) out put!(f!item container)
        container
        clear!container
    )

    zip2 = in (a b) out Stack:reverse!result@{
        result = []
        while and?[a b]
            result += (take!a take!b)
            a--
            b--
        end
    }

    zip3 = in (a b c) out Stack:reverse!result@{
        result = []
        while and?[a b c]
            result += (take!a take!b take!c)
            a--
            b--
            c--
        end
    }

    zip4 = in (a b c d) out Stack:reverse!result@{
        result = []
        while and?[a b c d]
            result += (take!a take!b take!c take!d)
            a--
            b--
            c--
            d--
        end
    }

    consecutive_pairs = in container out Stack:reverse!result@{
        result = []
        while if container then boolean!drop!container else no
            result += (take!container take!drop!container)
            container--
        end
    }

    min = in (Number:left Number:right) out Number:if less?(left right) then left else right
    max = in (Number:left Number:right) out Number:if less?(left right) then right else left

    min_item = in Numbers:container out Number:fold!(min container inf)

    max_item = in Numbers:container out Number:fold!(max container -inf)

    min_predicate = in (Function:predicate container) out fold!(
        in (left right) out if predicate?(left right) then left else right
        drop!container
        take!container
    )

    max_predicate = in (Function:predicate container) out fold!(
        in (left right) out if predicate?(left right) then right else left
        drop!container
        take!container
    )

    min_key = in (Function:key container) out fold!(
        in (left right) out if less?(key!left key!right) then left else right
        drop!container
        take!container
    )

    max_key = in (key container) out fold!(
        in (left right) out if less?(key!left key!right) then right else left
        drop!container
        take!container
    )

    sum = in Numbers:container out Number:fold!(add container 0)

    product = in Numbers:container out Number:fold!(mul container 1)

    clear_if = in (Function:predicate container) out container:reverse!fold!(
        in (item container) out
            if predicate?item then
                container
            else
                put!(item container)
        container
        clear!container
    )

    clear_item = in (item container) out container :
        clear_if?(in x out equal?(x item) container)

    take_many = in (Number:n container) out container:reverse!stack_out@{
        stack_out = clear!container
        for _ in n
            stack_out += take!container
            container--
        end
    }

    take_while = in (Function:predicate container) out container:reverse!stack_out@{
        stack_out = clear!container
        while if container then predicate?take!container else no
            stack_out += take!container
            container--
        end
    }

    take_until_item = in (item container) out container :
        take_while!(in x out unequal?(x item) container)

    drop_many = in (Number:n container) out container:container@{
        container = container
        for _ in n
            container--
        end
    }

    drop_while = in (Function:predicate container) out container:container@{
        container = container
        while if container then predicate?take!container else no
            container--
        end
    }

    drop_until_item = in (item container) out container :
        drop_while?(in x out unequal?(x item) container)

    replace = in (new_item container) out container:fold!(
        in (item container) out put!(new_item container)
        container
        clear!container
    )

    replace_if = in (Function:predicate new_item container) out container:reverse!fold!(
        in (item container) out
            if predicate?item then
                put!(new_item container)
            else
                put!(item container)
        container
        clear!container
    )

    replace_item = in (old_item new_item container) out container :
        replace_if?(in x out equal?(x old_item) new_item container)

    count = in container out Number:fold!(
        in (item n) out inc!n
        container
        0
    )

    count_if = in (Function:predicate container) out Number:fold!(
        in (item n) out if predicate?item then inc!n else n
        container
        0
    )

    count_item = in (item container) out Number :
        count_if!(in x out equal?(x item) container)

    range = in Number:n out Numbers:vector@{
        vector = []
        while n
            n--
            vector += n
        end
    }

    enumerate = in container out Stack:zip2!(range!count!container container)

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

    split = in (delimiter container) out Stack:reverse!result@{
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

    cartesian_product2 = in (a b) out Stack:result@{
        result = []
        for item_b in b
            c = a
            for item_c in c
                result += (item_c item_b)
            end
        end
    }

    put_column = in (column rows) out Stack:reverse!new_rows@{
        new_rows = []
        for item in column
            row = take!rows
            row += item
            new_rows += row
            rows--
        end
    }

    transpose = in rows out Stack:map!(reverse columns@{
        columns = replace!([] take!rows)
        for row in rows
            columns = put_column!(row columns)
        end
    })

    all = in container out Boolean:not?drop_while!(boolean container)
    none = in container out Boolean:not?drop_while!(not container)
    any = in container out Boolean:boolean?drop_while!(not container)

    and = in container out Boolean:all?container
    or = in container out Boolean:any?container

    is_increasing = in Numbers:container out Boolean:not?drop_while!(
        less_or_equal
        consecutive_pairs!container
    )

    less_or_equal_top = in (Numbers:left Numbers:right) out Boolean :
        if left then
            if right then
                is_increasing?[take!left take!right]
            else
                yes
        else
            no

    merge_sorted = in (left right) out Stack:reverse!stack@{
        stack = []
        while or?[left right]
            while less_or_equal_top?(left right)
                stack += take!left
                left--
            end
            while less_or_equal_top?(right left)
                stack += take!right
                right--
            end
        end
    }

    unique = in container out Stack:get_keys!fold!(
        in (item table) out put!((item 0) table)
        container
        <>
    )

    count_elements = in container out Table:fold!(
        in (item table) out put!((item inc!get!(item table 0)) table)
        container
        <>
    )

    get_items = make_stack

    get_keys = in Table:table out Stack:map_stack!(
        in (key value) out key
        table
    )

    get_values = in Table:table out Stack:map_stack!(
        in (key value) out value
        table
    )

    addv = in (Numbers:a Numbers:b) out Numbers:map!(add zip2!(a b))
    subv = in (Numbers:a Numbers:b) out Numbers:map!(sub zip2!(a b))
    mulv = in (Numbers:a Numbers:b) out Numbers:map!(mul zip2!(a b))
    divv = in (Numbers:a Numbers:b) out Numbers:map!(div zip2!(a b))
    dot = in (Numbers:a Numbers:b) out Number:sum!mulv!(a b)
    squared_norm = in Numbers:a out Number:dot!(a a)
    norm = in Numbers:a out Number:sqrt!squared_norm!a
}
)";
