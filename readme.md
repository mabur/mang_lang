# Mang Lang

Mang Lang is a toy programming language with the following **design trade-offs**:

1. Minimalistic instead of feature-rich. The implementation is ~500 lines of Python code.
2. Functional instead of imperative or object oriented.

**What it has**:

1. Numbers and strings are primitive data types.
2. Lists.
3. Dictionaries of symbols/variables.
4. Functions.
5. Conditional expressions (if then else).
6. Looping with recursive functions.
7. Looping with list comprehensions (each for in if).

**What it does NOT have**:

1. Mutable variables. Everything is immutable / constant.
2. Operators for arithmetic and boolean operations etc. Functions are used instead of operators.
3. Object oriented stuff like constructors, destructors, inheritance. Dictionaries are used for simple objects.

**Example code**:

```(
{
factorial = from x to 
    if check_equality [x, 0] then
        1
    else
        mul [x, factorial sub [x, 1]],
result = factorial 4
}
```
This example program defines two symbols: `factorial` which is a function and `result` which is a number. Symbols are always defined within a dictionary `{factorial = ..., result = ...}`. White space and new lines are optional.

Functions are defined using the syntax `from ... to ...`, and called using the syntax `function input`.
Mang Lang has built in functions like `check_equality`, `mul`, `sub` instead of having operators like `==`, `*` , `-`. Functions take a single input and a single output. Multiple things are passed to and from functions by first putting them in either a list `[]` or a dictionary `{}`.

In the example above the result gets the value `1*2*3*4=24`.

Another example of how Mang Lang uses dictionaries both for structuring a program into multiple lines and creating objects/records:

```
{
rectangle = {width = 4, height = 5},
area = mul [width rectangle, height rectangle]
}
```

In this example area gets the value `4*5=20`.
Note that the same syntax is used to apply a function and access a field in a dictionary: `function input` and `field dictionary`.

# Examples

1. [Numbers and Built-in Functions](#numbers-and-built-in-functions)
2. [Dictionaries and Variables](#dictionaries-and-variables)
3. [Lists](#lists)
4. [Functions and Control Flow](#functions-and-control-flow)
6. [List Comprehension](#list-comprehension)
7. [Strings](#Strings)
8. [Importing code in different source files](#importing-code-in-different-source-files)
9. [List of built-in functions](#list-of-built-in-functions)

## Numbers and Built-in Functions

Mang Lang can be used as a calculator. This is a trivial program that just contains a single number:

```
8
```
This program is unsurprisingly evaluated to the value  `8`.
Lists of numbers are created like this:
```
[7,3,4]
```
Computations are done by calling functions:
```
add [7,3,4]
```
This program is evaluated to `14`. Mang Lang does not have any operators for arithmetics etc, but instead uses functions for all computations. Function calls can be nested like this:

````
mul [add [1,2,3], div [7,2]]
````

This program is evaluated to `(1+2+3)*(7-2) = 3*5 = 15`.

## Dictionaries and Variables

Mang Lang uses dictionaries as flexible building blocks for many different things. Variables can be defined and used like this:

```
{
x=3,
y=2,
z=mul [x,y]
}
```
This program defines three variables, inside a dictionary `{}`. When evaluating this program `z` gets the value `6`. White space and new lines are optional in Mang Lang so the same program can also be written:
```
{x = 3, y = 2, z = mul [x, y]}
```
Dictionaries can be nested:
```
{
rectangle = {width = 4, height = 5},
area = mul [width rectangle, height rectangle]
}
```
Fields inside a dictionary can be retrieved using the syntax `element dictionary`.
Thus, the same syntax is used to retrieve an element from a dictionary as calling a function.

## lists

Example program:
```
{
list = [1, 3],
one = first list,
three = last list,
result1 = add [first list, last list],
result2 = add list,
dubble_list = concat [list, list],
s = size dubble_list
}
```
which is evaluated to:
```
{
list = [1, 3],
one = 1,
three = 3,
result1 = 4,
result2 = 4,
dubble_list = [1, 3, 1, 3],
s = 4
}
``` 

## Functions and Control Flow

Functions are defined using they keywords `from` and `to` like this:

```
{
square = from x to mul [x, x],
result = square 3
}
```

The if-then-else operator is used to choose what value to return based on a condition.
Recursive function calls and the if-then-else operator are used for loops:

```
{
factorial = from x to 
    if check_equality [x, 0] then
        1
    else
        mul [x, factorial sub [x, 1]],
result = factorial 4
}
```
Function definitions and computations can be broken up into smaller parts by using dictionaries:

```
{
square_norm = from vec2 to result {
    x = first vec2,
    y = last vec2,
    x2 = mul [x, x],
    y2 = mul [y, y],
    result = add [x2, y2]
    }
vector = [3, 4],
result = square_norm vector
}
```

## list Comprehension

In Mang Lang you can create loops by using recursion. list comprehension is an alternative way to express some loops in a simpler way. It is similar to "set builder notation" and "map" and "filter" operations. It is designed for the use case when you create a list from another list. It uses the syntax `each ... for ... in ...`:
```
{
input = [1, 2, 3],
output = each mul [x, x] for x in input
}
```
This computes the square of all elements in `input`. So `output` becomes `[1, 4, 9]`. Tuple comprehensions also support an optional `if` expression at the end. This can be used to filter values:
```
{
input = [1, 2, 3],
output = each mul [x, x] for x in input if check_inequality [x, 2]
}
```
In this example `output` becomes `[1, 9]`.

## Strings
Strings are similar to lists: 
```
{
first_name = "Magnus",
last_name = "Burenius",
full_name = concat [first_name, " ", last_name],
initials = concat [first first_name, first last_name]
}
```
In this example `full_name` gets the value "Magnus Burenius" and `initials` gets the value "MB".

## Importing code in different source files

Source code can be put in different source files. If you have a file called `math` that contains the following:
```
{
square = from x to mul [x, x],
pi = 3.14151965
}
```
then you can import those definitions into another source file by using the `import` function:
```
{
math   = import "math",
tau    = mul [2, pi math],
square = square math,
four   = square 2
}
```

## List of built-in functions

### Arithmetic functions:
* **add**: adds a list of numbers.
* **mul**: multiplies a list of numbers.
* **sub**: subtracts two numbers.
* **div**: divides two numbers.
* **min**: smallest number in a non-empty list.
* **max**: largest number in a non-empty list.

### Logical functions:
* **check_equality**: true if two numbers are equal, and otherwise false.
* **check_inequality**: false if two numbers are equal, and otherwise true.
* **all**: true if all elements of a list are true.
* **any**: true if at least one element of a list is true.
* **none**: true if all elements of a list are false.

### List and string functions
* **size**: the number of elements of a list or string.
* **is_empty**: true if a list or string has zero elements.
* **concat**: concatenates two lists or strings.
* **first**: pick the first element.
* **last**: pick the last element.
* **first_part**: pick all elements except the last.
* **last_part**: pick all elements except the first.
