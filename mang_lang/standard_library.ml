{
    find = from input to
        if is_empty of list of input then
            [[],[]]
        else
            if check_equality of [first of list of input, query of input]
                then [[], list of input]
                else result of {
                    list = list of input,
                    query = query of input,
                    recursive_result = find of {list=last_part of list, query=query}
                    head = concat of [[first of list], first of recursive_result],
                    tail = last of recursive_result,
                    result = [head, tail]
                }
}
