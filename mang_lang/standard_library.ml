{
    count = from input to
        if is_empty input then
            0
        else
            add [1, count last_part input],
    count_item = from input to
        if is_empty list<input then
            0
        else
            result<{
                item = item<input,
                list = list<input,
                head = first list,
                tail = last_part list,
                x = if equal [head, item] then 1 else 0,
                result = add [x, count_item {list=tail, item=item}],
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
        else if equal [first list<input, item<input] then
            [[], list<input]
        else
            result<{
                list = list<input,
                item = item<input,
                recursive_result = find {list = last_part list, item = item},
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
                back = f last list,
                beginning = map {list = first_part list, f = f},
                result = append{list=beginning, item=back}
            },
    filter = from input to
        if is_empty list<input then
            []
        else
            result<{
                list = list<input,
                predicate = predicate<input,
                back = last list,
                beginning = filter {list = first_part list, predicate = predicate},
                result =
                    if predicate back then
                        append{list=beginning, item=back}
                    else
                        beginning
            },
    enumerate = from list to
        if is_empty list then
            []
        else
            result<{
                index = sub [count list, 1],
                back = {item = last list, index = index},
                beginning = enumerate first_part list,
                result = append{list=beginning, item=back}
            },
    reverse = from list to
        if is_empty list then
            list
        else
            concat[reverse last_part list, [first list]],
    split = from input to 
        if is_empty list<input then
            []
        else
            result<{
                division = find {list = list<input, item = separator<input},
                recursive_result = split {list = last_part last division, separator = separator<input},
                result = concat [[first division], recursive_result]
            },
    get_index = from input to
        if equal [index<input, 0] then
            first list<input
        else
            get_index {
                list = last_part list<input,
                index = sub[index<input, 1]
            },
    get_wrapped_index = from input to
        result<{
            index = index<input,
            list = list<input,
            count = count list<input,
            result = 
                if increasing [index, 0] then
                    get_wrapped_index {list = list, index = add [index, count]}
                else if weakly_increasing [count, index] then
                    get_wrapped_index {list = list, index = sub [index, count]}
                else
                    get_index {list = list, index = index}
        }
}
