Day1:

cartProd _ []=[]
cartProd [] _ = []
cartProd (x:xs) (y:ys) = [(x,y)] ++ cartProd [x] ys  ++ cartProd xs ys ++  cartProd xs [y]

result<{
    list = [1721,979,366,299,675,1456],
    product_of_selected_pairs = from input to
        if is_empty first input then
            1
        else if is_empty last input then
            1
        else
            result<{
                left = first input,
                right = last input,
                left_head = first left,
                left_tail = last_part left,
                right_head = first right,
                right_tail = last_part right,
                current =
                    if equal[add[left_head, right_head], 2020] then
                        mul[left_head, right_head]
                    else
                        1,
                result = mul [
                    current,
                    product_of_selected_pairs[[left_head], right_tail],
                    product_of_selected_pairs[[right_head], left_tail],
                    product_of_selected_pairs[left_tail, right_tail]
                ]
            },
    square = product_of_selected_pairs[list, list],
    result = sqrt square
}

cartProd _ []=[]
cartProd [] _ = []
cartProd (x:xs) (y:ys) = [(x,y)] ++ cartProd [x] ys  ++ cartProd xs ys ++  cartProd xs [y]

cartesian_product = from input to
    result<{
        left = first input,
        right = last input,
        map_left = from item_right to map{
            list = left,
            f = from item_left to [item_left, item_right]
        },
        result = map{list=right, f=map_left}
    }

Day3:
result<{
    map = [
            [0,0,1,1,0,0,0,0,0,0,0],
            [1,0,0,0,1,0,0,0,1,0,0],
            [0,1,0,0,0,0,1,0,0,1,0],
            [0,0,1,0,1,0,0,0,1,0,1],
            [0,1,0,0,0,1,1,0,0,1,0],
            [0,0,1,0,1,1,0,0,0,0,0],
            [0,1,0,1,0,1,0,0,0,0,1],
            [0,1,0,0,0,0,0,0,0,0,1],
            [1,0,1,1,0,0,0,1,0,0,0],
            [1,0,0,0,1,1,0,0,0,0,1],
            [0,1,0,0,1,0,0,0,1,0,1]
        ],
    count_trees = from input to
        if is_empty map<input then
            0
        else
            result<{
                map = map<input,
                row = first map,
                rows = last_part map,
                width = count row,
                index = index<input,
                updated_index = add[index, 3],
                next_index =
                    if increasing[updated_index, width] then
                        updated_index
                    else
                        sub[updated_index, width],
                item = get_index {list=row, index=index},
                result = add [item, count_trees {map=rows, index=next_index}]
            },
    result = count_trees{map = map, index = 0}
}
