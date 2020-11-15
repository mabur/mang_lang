{
    count = from input to
        if is_empty input then
            0
        else
            add [1, count last_part input],
    count_value = from input to
        if is_empty list<input then
            0
        else
            result<{
                value = value<input,
                list = list<input,
                head = first list,
                tail = last_part list,
                x = if equal [head, value] then 1 else 0,
                result = add [x, count_value {list=tail, value=value}],
            },
    count_if = from input to
        if is_empty list<input then
            0
        else
            result<{
                predicate = predicate<input,
                list = list<input,
                head = first list,
                tail = last_part list,
                x = if predicate head then 1 else 0,
                result = add [x, count_if {list=tail, predicate=predicate}],
            },
    find = from input to
        if is_empty list<input then
            [[], []]
        else if equal [first list<input, value<input] then
            [[], list<input]
        else
            result<{
                list = list<input,
                value = value<input,
                recursive_result = find {list = last_part list, value = value},
                head = concat [[first list], first recursive_result],
                tail = last recursive_result,
                result = [head, tail]
            },
    find_if = from input to
        result<{
            list = list<input,
            predicate = predicate<input,
            result = 
                if is_empty list then
                    [[], []]
                else if predicate first list then
                    [[], list]
                else
                    subresult<{
                        recursive_result = find_if {list = last_part list, predicate = predicate},
                        head = concat [[first list], first recursive_result],
                        tail = last recursive_result,
                        subresult = [head, tail]
                    }
        },
    map = from input to
        if is_empty list<input then
            []
        else
            result<{
                list = list<input,
                f = f<input,
                head = f first list,
                tail = map {list = last_part list, f = f},
                result = concat [[head], tail]
            },
    filter = from input to
        if is_empty list<input then
            []
        else
            result<{
                list = list<input,
                predicate = predicate<input,
                head = first list,
                tail = filter {list = last_part list, predicate = predicate}
                result =
                    if predicate head then
                        concat [[head], tail]
                    else
                        tail
            }
}
