# Mang Lang

Mang Lang is a toy programming language with the following **design trade-offs**:

1. Minimalistic instead of feature-rich.
2. Functional instead of imperative or object oriented.

**What it has**:

1. Numbers and strings are primitive data types.
3. Tuples are used for composite data types.
4. Variables.
5. Functions.
6. Conditional expressions (if then else).
7. Looping with tuple comprehensions (all for in if).
8. Looping with recursive functions.


**What it does NOT have**:

1. Mutable variables. Everything is immutable / constant.
2. Operators for arithmetic and boolean operations etc. Functions are used instead of operators.
3. Object oriented stuff like constructors, destructors, inheritance. Tuples are used for simple objects.

**Example code**:

```(
(
faculty = from x to if equal(x,0) then 1 else mul(x,faculty(sub(x,1))),
result = faculty(4)
)
```

This example first defines a function named `faculty` that takes an argument named `x`. The function is defined recursively. It then defines a variable named `result` to be the value of `faculty(4)` i.e. `1*2*3*4=24`. Mang Lang uses functions like `equal`, `mul`, `sub` instead of having operators like `==`, `*` , `-`. Whitespace and new lines are optional.

# Examples

1. [Numbers and Built-in Functions](#numbers-and-built-in-functions)
2. [Variables and Multiple Statements](#variables-and-multiple-statements)
3. [Tuples](#tuples)
4. [Functions](#functions)
5. [Scope](#scope)
6. [Tuple Comprehension](#tuple-comprehension)
7. [Strings](#Strings)
8. [Importing code in different source files](#importing-code-in-different-source-files)
9. [List of built-in functions](#list-of-built-in-functions)

## Numbers and Built-in Functions

Mang Lang can be used as a calculator. This is a trivial program that just contains a single number:

```
8
```

This program is unsurprisingly evaluated to the value  `8`. Mang Lang does not have any operators for arithmetics etc, but instead uses functions:

```
add(8,3)
```

This program is evaluated to `11`. Function calls can be nested like this:

````
mul(add(1,2),sub(3,1))
````

This program is evaluated to `6`.

## Variables and Multiple Statements

Variables can be defined and used like this:

```
(
x=3,
y=2,
z=mul(x,y)
)
```

This program is made up of three variable definitions. When evaluating this program z gets the value `6`. Tuples are used to separate expressions/statements:

```
(
statement1,
statement2,
statement3
)
```

White space and new lines are optional in Mang Lang so the same program can also be written:

```(
(statement1,statement2,statement3)
```

and in our concrete example:

```(
(x = 3, y = 2, z = mul(x, y))
```

## Tuples

Mang Lang uses tuples as flexible building blocks for many different things. White space and new lines are optional everywhere. Example using tuples to create a program with multiple "lines" of "statements":

```
(
x = 1,
y = 3,
result = add(x, y)
)
```

Example using tuples as lists or arrays:

```
(
array = (1, 3),
result = add(array[0], array[1])
)
```

Example using tuples to create data records/objects:

```
(
object = (x = 1, y = 3),
result = add(object.x, object.y)
)
```

## Functions

Functions are defined using they keywords `from` and `to` like this:

```
(
square_norm = from x to add(mul(x[0], x[0]), mul(x[1], x[1])),
vector = (3, 4),
result = square_norm(vector)
)
```

The if-then-else operator is used to choose what value to return based on a condition:

```
(
abs = from x to if greater(x, 0) then x else sub(0, x)
result = abs(-10)
)
```

Recursive function calls and the if-then-else operator are used for loops:

```
(
faculty = x from to if equal(x, 0) then 1 else mul(x, faculty(sub(x, 1))),
result = faculty(10)
)
```

Function definitions can be broken up into smaller parts that are put in a local scope using the `where` keyword:

```
(
square_norm = from v where (
    x2 = mul(v[0], v[0]),
    y2 = mul(v[1], v[1])
    ) to add(x2, y2),
vector = (3, 4),
result = square_norm(vector)
)
```

## Tuple Comprehension

In Mang Lang you can create loops by using recursion.
Tuple comprehension is an alternative way to express some loops in a simpler way.
It is similar to "set builder notation" and "map" and "filter" operations.
It is designed for the use case when you create a tuple by looping over another tuple:

```
(
input = (1, 2, 3),
output = all mul(x, x) for x in input
)
```
This computes the square of all elements in `input`.
So `output` becomes `(1, 4, 9)`.
Tuple comprehensions also support an optional `if` expression at the end. This can be used to filter values:
```
(
input = (1, 2, 3),
output = all mul(x, x) for x in input if not(equal(x, 2))
)
```
In this example `output` becomes `(1, 9)`.

## Strings
Strings are similar to tuples: 
```
(
first_name = "Magnus",
last_name = "Burenius",
full_name = concat(first_name, " ", last_name),
initials = concat(first_name[0], last_name[0])
)
```
In this example `full_name` gets the value "Magnus Burenius" and `initials` gets the value "MB".

## Importing code in different source files

Source code can be put in different source files. If you have a file called `math` that contains the following:
```
(
square = from x to mul(x, x),
pi = 3.14151965
)
```
then you can import those definitions into another source file by using the `import` function:
```
(
math = import("math"),
tau = mul(2, math.pi),
four = math.square(2)
)
```

## List of built-in functions

### Arithmetic functions:
* **add**: adds two numbers.
* **mul**: multiplies two numbers.
* **sub**: subtracts two numbers.
* **div**: divides two numbers.

### Logical functions:
* **equal**: true if two numbers are equal, and otherwise false.
* **and**: true if two variables are both true, and otherwise false.
* **or**: true if at least one of two variables are true, and otherwise false.
* **not**: true if a variable is false, and otherwise false.

### Tuple and string functions
* **size**: the number of elements of a tuple or string.
* **is_empty**: true if a tuple or string has zero elements.
* **concat**: concatenates two tuples or strings.

### Tuple of numbers
* **sum**: sum all numbers in a tuple. Gives zero if the tuple is empty.
* **min**: smallest number in a non-empty tuple.
* **max**: largest number in a non-empty tuple.
