# Mang Lang

Mang Lang is a toy programming language with the following **design trade-offs**:

1. Minimalistic instead of feature-rich. The implementation is ~500 lines of Python code.
2. Functional instead of imperative or object oriented.

**What it has**:

1. Numbers and strings are primitive data types.
3. Arrays.
4. Dictionaries of symbols/variables.
5. Functions.
6. Conditional expressions (if then else).
7. Looping with recursive functions.
8. Looping with array comprehensions (each for in if).

**What it does NOT have**:

1. Mutable variables. Everything is immutable / constant.
2. Operators for arithmetic and boolean operations etc. Functions are used instead of operators.
3. Object oriented stuff like constructors, destructors, inheritance. Dictionaries are used for simple objects.

**Example code**:

```(
{
faculty = from x to 
    if check_equality of [x, 0] then 1
    else product of [x, faculty of difference of [x, 1]],
result = faculty of 4
}
```
This example program defines two symbols: `faculty` which is a function and `result` which is a number. Symbols are always defined within a dictionary `{faculty = ..., result = ...}`.
White space and new lines are optional.

Functions are defined using the syntax `from ... to ...`, and called using the syntax `function of input`.
Mang Lang has built in functions like `check_equality`, `product`, `difference` instead of having operators like `==`, `*` , `-`.
Functions take a single input and a single output.
Multiple things are passed to and from functions by first putting them in either an array `[]` or a dictionary `{}`.

In this example result gets the value `1*2*3*4=24`.

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
This program is unsurprisingly evaluated to the value  `8`.
Arrays of numbers are created like this:
```
[7,3,4]
```
Computations are done by calling functions:
```
sum of [7,3,4]
```
This program is evaluated to `14`.
Mang Lang does not have any operators for arithmetics etc, but instead uses functions for all computations.
Function calls can be nested like this:

````
product of [sum of [1,2,3], difference of [7,2]]
````

This program is evaluated to `(1+2+3)*(7-2) = 3*5 = 15`.

## Dictionaries and Variables

Mang Lang uses dictionaries as flexible building blocks for many different things.
Variables can be defined and used like this:
```
{
x=3,
y=2,
z=product of [x,y]
}
```
This program defines three variables, inside a dictionary `{}`.
When evaluating this program `z` gets the value `6`. White space and new lines are optional in Mang Lang so the same program can also be written:
```
{x = 3, y = 2, z = product of [x, y]}
```
Dictionaries can be nested:
```
{
rectangle = {width = 4, height = 5},
area = product of [width of rectangle, height of rectangle]
}
```
Variables inside a dictionary can be retrieved using the syntax `element of dictionary`.
Thus, the same syntax is used to retrieve an element from a dictionary as calling a function.

## Arrays

Example program:
```
{
array = [1, 3],
one = 0 of array,
three = 1 of array,
result1 = sum of [0 of array, 1 of array],
result2 = sum of array,
dubble_array = concat of [array, array],
s = size of dubble_array
}
```
which is evaluated to:
```
{
array = [1, 3],
one = 1,
three = 3,
result1 = 4,
result2 = 4,
dubble_array = [1, 3, 1, 3],
s = 4
}
```
Element `i` of an array is retrieved using the syntax `i of array`.
Note that this is the same syntax as retrieving a variable from a dictionary and calling a function. 

## Functions and Control Flow

Functions are defined using they keywords `from` and `to` like this:

```
{
square = from x to product of [x, x],
result = square of 3
}
```

The if-then-else operator is used to choose what value to return based on a condition.
Recursive function calls and the if-then-else operator are used for loops:

```
{
faculty = x from to 
    if check_equality of [x, 0] then 1
    else product of [x, faculty of difference of [x, 1]],
result = faculty of 4
}
```
Function definitions and compuations can be broken up into smaller parts by using dictionaries:

```
{
square_norm = from vec2 to result of {
    x = 0 of vec2,
    y = 1 of vec2,
    x2 = product of [x, x],
    y2 = product of [y, y],
    result = sum of [x2, y2]
    }
vector = [3, 4],
result = square_norm of vector
}
```

## Array Comprehension

In Mang Lang you can create loops by using recursion.
Array comprehension is an alternative way to express some loops in a simpler way.
It is similar to "set builder notation" and "map" and "filter" operations.
It is designed for the use case when you create an array from another array.
It uses the syntax `each ... for ... in ...`:
```
{
input = [1, 2, 3],
output = each product of [x, x] for x in input
}
```
This computes the square of all elements in `input`.
So `output` becomes `[1, 4, 9]`.
Tuple comprehensions also support an optional `if` expression at the end. This can be used to filter values:
```
{
input = [1, 2, 3],
output = each product of [x, x] for x in input if check_inequality of [x, 2]
}
```
In this example `output` becomes `[1, 9]`.

## Strings
Strings are similar to tuples: 
```
{
first_name = "Magnus",
last_name = "Burenius",
full_name = concat of [first_name, " ", last_name],
initials = concat of [0 of first_name, 0 of last_name]
}
```
In this example `full_name` gets the value "Magnus Burenius" and `initials` gets the value "MB".

## Importing code in different source files

Source code can be put in different source files. If you have a file called `math` that contains the following:
```
{
square = from x to product of [x, x],
pi = 3.14151965
}
```
then you can import those definitions into another source file by using the `import` function:
```
{
math   = import of "math",
tau    = product of [2, pi of math],
square = square of math,
four   = square of 2
}
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
* **check_equality**: true if two numbers are equal, and otherwise false.
* **check_inequality**: false if two numbers are equal, and otherwise true.
* **all**: true if all elements of a tuple are true.
* **any**: true if at least one element of a tuple is true.
* **none**: true if all elements of a tuple are false.

### Array and string functions
* **size**: the number of elements of a tuple or string.
* **is_empty**: true if a tuple or string has zero elements.
* **concat**: concatenates two tuples or strings.
