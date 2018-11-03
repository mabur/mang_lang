Mang lang is a toy programming language that is designed to be minimalistic, simple and consistent. It is also functional. Mang lang is made up of the following building blocks:
1. Numbers
2. Variables
3. Functions
4. Tuples
5. Conditional operator

Things it does NOT contain:
* Mutable variables. Everything is immutable.
* Control structures like: for, while, switch. Recursion and the conditional operator is used for loops.
* Object oriented stuff like constructors, destructors, inheritance. Tuples are used for simple objects.
* Operators for arithmetic and boolean operations etc. Functions are used instead of operators.


BASIC NUMBERS AND VARIABLES

Examples:
3
x = 3


BASIC TUPLES

Examples:
(1, 7, 3)
x = (1, 7, 3)


ADDITIONAL USAGES OF TUPLES

In Mang lang tuples are used as fundamental building blocks that are used for many different things.
White space and new lines are optional everywhere.
Example using tuples to create a program with multiple "lines" of "statements":
(
x = 1,
y = 3,
result = add(x, y)
)
Example using tuples as lists or arrays:
(
array = (1, 3),
result = add(array[0], array[1])
)
Example using tuples to create data records/objects:
(
object = (x = 1, y = 3),
result = add(object.x, object.y)
)


FUNCTIONS

(
square_norm(x) = add(mul(x[0], x[0]), mul(x[1], x[1])),
vector = (3, 4),
result = square_norm(vector)
)

(
abs(x) = if greater(x, 0) then x else sub(0, x)
result = abs(-10)
)

(
faculty(x) = if equal(x, 0) then 1 else mul(x, faculty(sub(x, 1))),
result = faculty(10)
)


SCOPE

(
square_norm(v) = {
    x2 = mul(v[0], v[0]),
    y2 = mul(v[1], v[1])
} = add(x2, y2),
vector = (3, 4),
result = square_norm(vector)
)


