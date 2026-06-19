#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    Any = dynamic 0
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
    serialize_digit = in Number:x out Character:character!add!(x number!'0')

    parse_natural_number = in String:string out Number:number@{
        reversed_string = reverse!string
        number = 0
        x = 1
        for c in reversed_string
            digit = parse_digit!c
            number = add!(number mul!(x digit))
            x = mul!(x 10)
        end
    }

    serialize_natural_number = in Number:number out String:string@{
        x = number        
        string = ""
        string += serialize_digit!mod!(x 10)
        x = round_down!div!(x 10)
        while less?(0 x) 
            string += serialize_digit!mod!(x 10)
            x = round_down!div!(x 10)
        end
    }

    to_upper = in Character:c out Character:
        if is_lower?c then
            character!sub!(number!c 32)
        else
            c

    to_lower = in Character:c out Character:
        if is_upper?c then
            character!add!(number!c 32)
        else
            c

    fold = in (Function:operation in_stream init) out result@{
        result = init
        s = in_stream
        for item in s
            result = operation!(item result)
        end
    }

    put_each = in (in_stream out_stream) out out_stream:fold!(
        put
        in_stream
        out_stream
    )

    reverse = in container out container:put_each!(
        container
        clear!container
    )

    make_stack = in in_stream out Stack:reverse!put_each!(
        in_stream
        []
    )

    make_string = in in_stream out String:reverse!put_each!(
        in_stream
        ""
    )

    make_table = in in_stream out Table:put_each!(
        in_stream
        <>
    )

    merge_generic = in (in_streams out_stream) out out_stream:fold!(
        put_each
        in_streams
        out_stream
    )

    merge_stack = in in_streams out Stack:reverse!merge_generic!(
        in_streams
        []
    )

    merge_string = in in_streams out String:reverse!merge_generic!(
        in_streams
        ""
    )

    merge_table = in containers out Table:merge_generic!(
        reverse!containers
        <>
    )

    map_generic = in (Function:f in_stream out_stream) out fold!(
        in (item stream) out put!(f!item stream)
        in_stream
        out_stream
    )

    map = in (Function:f container) out reverse!map_generic!(
        f
        container
        clear!container
    )

    map_stack = in (Function:f in_stream) out Stack:reverse!map_generic!(
        f
        in_stream
        []
    )

    map_string = in (Function:f in_stream) out String:reverse!map_generic!(
        f
        in_stream
        ""
    )

    map_table = in (Function:f in_stream) out Table:map_generic!(
        f
        in_stream
        <>
    )

    zip2 = in (a b) out Stack:reverse!result@{
        a2 = a
        b2 = b
        result = []
        while and?[a2 b2]
            result += (take!a2 take!b2)
            a2--
            b2--
        end
    }

    zip3 = in (a b c) out Stack:reverse!result@{
        a2 = a
        b2 = b
        c2 = c
        result = []
        while and?[a2 b2 c2]
            result += (take!a2 take!b2 take!c2)
            a2--
            b2--
            c2--
        end
    }

    zip4 = in (a b c d) out Stack:reverse!result@{
        a2 = a
        b2 = b
        c2 = c
        d2 = d
        result = []
        while and?[a2 b2 c2 d2]
            result += (take!a2 take!b2 take!c2 take!d2)
            a2--
            b2--
            c2--
            d2--
        end
    }

    consecutive_pairs = in in_stream out Stack:reverse!result@{
        s = in_stream
        result = []
        while if s then boolean!drop!s else no
            result += (take!s take!drop!s)
            s--
        end
    }

    min = in (Number:left Number:right) out Number:if less?(left right) then left else right
    max = in (Number:left Number:right) out Number:if less?(left right) then right else left

    min_item = in Numbers:in_stream out Number:fold!(min in_stream inf)

    max_item = in Numbers:in_stream out Number:fold!(max in_stream -inf)

    min_predicate = in (Function:predicate in_stream) out fold!(
        in (left right) out if predicate?(left right) then left else right
        drop!in_stream
        take!in_stream
    )

    max_predicate = in (Function:predicate in_stream) out fold!(
        in (left right) out if predicate?(left right) then right else left
        drop!in_stream
        take!in_stream
    )

    min_key = in (Function:key in_stream) out fold!(
        in (left right) out if less?(key!left key!right) then left else right
        drop!in_stream
        take!in_stream
    )

    max_key = in (Function:key in_stream) out fold!(
        in (left right) out if less?(key!left key!right) then right else left
        drop!in_stream
        take!in_stream
    )

    sum = in Numbers:in_stream out Number:fold!(add in_stream 0)

    product = in Numbers:in_stream out Number:fold!(mul in_stream 1)

    clear_if = in (Function:predicate container) out container:reverse!fold!(
        in (item container) out
            if predicate?item then
                container
            else
                put!(item container)
        container
        clear!container
    )

    clear_item = in (item container) out container:
        clear_if?(in x out equal?(x item) container)

    take_many = in (Number:n container_in) out container_in:reverse!container_out@{
        container = container_in
        container_out = clear!container
        m = n
        for m
            container_out += take!container
            container--
        end
    }

    take_while = in (Function:predicate container_in) out container_in:reverse!container_out@{
        container = container_in        
        container_out = clear!container
        while if container then predicate?take!container else no
            container_out += take!container
            container--
        end
    }

    take_until_item = in (item container) out container:
        take_while!(in x out unequal?(x item) container)

    drop_many = in (Number:n in_stream) out in_stream:stream@{
        stream = in_stream
        m = n
        for m
            stream--
        end
    }

    drop_while = in (Function:predicate in_stream) out in_stream:stream@{
        stream = in_stream
        while if stream then predicate?take!stream else no
            stream--
        end
    }

    drop_until_item = in (item in_stream) out in_stream:
        drop_while?(in x out unequal?(x item) in_stream)

    replace = in (new_item container) out map!(
        in old_item out new_item
        container
    )

    replace_if = in (Function:predicate new_item container) out map!(
        in old_item out
            if predicate?old_item then
                new_item
            else
                old_item
        container
    )

    replace_item = in (old_item new_item container) out 
        replace_if?(in x out equal?(x old_item) new_item container)

    count = in in_stream out Number:fold!(
        in (item n) out inc!n
        in_stream
        0
    )

    count_if = in (Function:predicate in_stream) out Number:fold!(
        in (item n) out if predicate?item then inc!n else n
        in_stream
        0
    )

    count_item = in (item in_stream) out Number:
        count_if!(in x out equal?(x item) in_stream)

    range = in Number:n out Numbers:numbers@{
        numbers = []
        m = n
        for m
            numbers += dec!m
        end
    }

    enumerate = in in_stream out Stack:zip2!(range!count!in_stream in_stream)

    get0 = in in_stream out in_stream!0
    get1 = in in_stream out in_stream!1
    get2 = in in_stream out in_stream!2
    get3 = in in_stream out in_stream!3
    get4 = in in_stream out in_stream!4
    get5 = in in_stream out in_stream!5
    get6 = in in_stream out in_stream!6
    get7 = in in_stream out in_stream!7
    get8 = in in_stream out in_stream!8
    get9 = in in_stream out in_stream!9

    split = in (delimiter container) out Stack:reverse!result@{
        word = take_until_item!(delimiter container)
        sub_container = drop_until_item!(delimiter container)
        result = [word]
        while sub_container
            sub_container--
            word = take_until_item!(delimiter sub_container)
            sub_container = drop_until_item!(delimiter sub_container)
            result += word
        end
    }

    cartesian_product2 = in (a b) out Stack:result@{
        result = []
        d = b
        for item_d in d
            c = a
            for item_c in c
                result += (item_c item_d)
            end
        end
    }

    put_column = in (column rows) out Stack:reverse!new_rows@{
        remaining_rows = rows
        new_rows = []
        c = column
        for item in c
            row = take!remaining_rows
            row += item
            new_rows += row
            remaining_rows--
        end
    }

    transpose = in rows out Stack:map!(reverse columns@{
        columns = replace!([] take!rows)
        r = rows
        for row in r
            columns = put_column!(row columns)
        end
    })

    all = in in_stream out Boolean:not?drop_while!(boolean in_stream)
    none = in in_stream out Boolean:not?drop_while!(not in_stream)
    any = in in_stream out Boolean:boolean?drop_while!(not in_stream)

    and = in in_stream out Boolean:all?in_stream
    or = in in_stream out Boolean:any?in_stream

    is_increasing = in Numbers:numbers out Boolean:not?drop_while!(
        less_or_equal
        consecutive_pairs!numbers
    )

    less_or_equal_top = in (Numbers:left Numbers:right) out Boolean:
        if left then
            if right then
                is_increasing?[take!left take!right]
            else
                yes
        else
            no

    merge_sorted = in (left_in right_in) out Stack:reverse!stack@{
        left = left_in
        right = right_in
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

    unique = in in_stream out Stack:get_keys!fold!(
        in (item table) out put!((item 0) table)
        in_stream
        <>
    )

    count_elements = in in_stream out Table:fold!(
        in (item table) out put!((item inc!get!(item table 0)) table)
        in_stream
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
