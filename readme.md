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
7. Looping with tuple comprehensions (each for in if).
8. Looping with recursive functions.


**What it does NOT have**:

1. Mutable variables. Everything is immutable / constant.
2. Operators for arithmetic and boolean operations etc. Functions are used instead of operators.
3. Object oriented stuff like constructors, destructors, inheritance. Tuples are used for simple objects.

**Example code**:

```(
(
faculty = from x to 
    if equal of (x, 0) then 1
    else product of (x, faculty of difference of (x, 1)),
result = faculty of 4
)
```

This example first defines a function named `faculty` that takes an argument named `x`. The function is defined recursively. It then defines a variable named `result` to be the value of `faculty of 4` i.e. `1*2*3*4=24`. Mang Lang uses functions like `equal`, `product`, `difference` instead of having operators like `==`, `*` , `-`. Whitespace and new lines are optional.

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
sum of (8,3)
```

This program is evaluated to `11`. Function calls can be nested like this:

````
product of (sum of (1,2), difference of (3,1))
````

This program is evaluated to `(1+2)*(3-1)=6`.

## Variables and Multiple Statements

Variables can be defined and used like this:

```
(
x=3,
y=2,
z=product of (x,y)
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
(x = 3, y = 2, z = product of (x, y))
```

## Tuples

Mang Lang uses tuples as flexible building blocks for many different things. White space and new lines are optional everywhere. Example using tuples to create a program with multiple "lines" of "statements":

```
(
x = 1,
y = 3,
result = sum of (x, y)
)
```

Example using tuples as lists or arrays:

```
(
array = (1, 3),
one = 0 of array,
three = 1 of array,
result1 = sum of (0 of array, 1 of array),
result2 = sum of array,
)
```

Example using tuples to create data records/objects:

```
(
object = (x = 1, y = 3),
result = sum of (x of object, y of object)
)
```

## Functions

Functions are defined using they keywords `from` and `to` like this:

```
(
square = from x to product of (x, x)
square_norm = from vec2 to sum of (square of 0 of vec2, square of 1 of vec2),
vector = (3, 4),
result = square_norm of vector
)
```

The if-then-else operator is used to choose what value to return based on a condition:

```
(
negation = from x to difference of (0, x),
abs = from x to if greater of (x, 0) then x else negation of x,
result = abs of -10
)
```

Recursive function calls and the if-then-else operator are used for loops:

```
(
faculty = x from to 
    if equal of (x, 0) then 1
    else product of (x, faculty of difference of (x, 1)),
result = faculty of 10
)
```

Function definitions can be broken up into smaller parts that are put in a local scope using the `where` keyword:

```
(
square_norm = from vec2 where (
    x2 = product of (0 of vec2, 0 of vec2),
    y2 = product of (1 of vec2, 1 of vec2)
    ) to sum of (x2, y2),
vector = (3, 4),
result = square_norm of vector
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
output = each product of (x, x) for x in input
)
```
This computes the square of all elements in `input`.
So `output` becomes `(1, 4, 9)`.
Tuple comprehensions also support an optional `if` expression at the end. This can be used to filter values:
```
(
input = (1, 2, 3),
output = each product of (x, x) for x in input if none of (equal of (x, 2))
)
```
In this example `output` becomes `(1, 9)`.

## Strings
Strings are similar to tuples: 
```
(
first_name = "Magnus",
last_name = "Burenius",
full_name = concat of (first_name, " ", last_name),
initials = concat of (0 of first_name, 0 of last_name)
)
```
In this example `full_name` gets the value "Magnus Burenius" and `initials` gets the value "MB".

## Importing code in different source files

Source code can be put in different source files. If you have a file called `math` that contains the following:
```
(
square = from x to product of (x, x),
pi = 3.14151965
)
```
then you can import those definitions into another source file by using the `import` function:
```
(
math = import of "math",
tau  = product of (2, pi of math),
square = square of math,
four = square of 2
)
```

## List of built-in functions

### Arithmetic functions:
* **sum**: adds a tuple of numbers.
* **product**: multiplies a tuple of numbers.
* **difference**: subtracts two numbers.
* **division**: divides two numbers.
* **min**: smallest number in a non-empty tuple.
* **max**: largest number in a non-empty tuple.

### Logical functions:
* **equal**: true if two numbers are equal, and otherwise false.
* **all**: true if all elements of a tuple are true.
* **any**: true if at least one element of a tuple is true.
* **none**: true if all elements of a tuple are false.

### Tuple and string functions
* **size**: the number of elements of a tuple or string.
* **is_empty**: true if a tuple or string has zero elements.
* **concat**: concatenates two tuples or strings.

