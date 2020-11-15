{
    count = from input to
        if is_empty input then
            0
        else
            add [1, count last_part input],
    find = from input to
        if is_empty list<input then
            [[], []]
        else if equal [first list<input, query<input] then
            [[], list<input]
        else
            result<{
                list = list<input,
                query = query<input,
                recursive_result = find {list = last_part list, query = query}
                head = concat [[first list], first recursive_result],
                tail = last recursive_result,
                result = [head, tail]
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
            }
}
