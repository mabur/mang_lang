# Advent of code 2017 exercise 1.
# Recursive Python solution:

a = [1,1,2,2]
b = [1,1,1,1]
c = [1,2,3,4]
d = [9,1,2,1,2,1,2,9]


def recursive(count, collection):
    if len(collection) == 1:
        return count, []
    if collection[0] == collection[1]:
        return recursive(count + collection[0], collection[1:])
    return recursive(count, collection[1:])

def wrapper(collection):
    return recursive(0, collection + [collection[0]])


print(wrapper(a))
print(wrapper(b))
print(wrapper(c))
print(wrapper(d))
