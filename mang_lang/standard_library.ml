{
    find = from input to
        if is_empty list<input then
            [[],[]]
        else
            if check_equality [first list<input, query<input]
                then [[], list<input]
                else result<{
                    list = list<input,
                    query = query<input,
                    recursive_result = find {list=last_part list, query=query}
                    head = concat [[first list], first recursive_result],
                    tail = last recursive_result,
                    result = [head, tail]
                }
}
