# Mang Lang

Mang Lang is an experimental minimalistic programming language.
Its primary goal is to be a simple and minimal language that is easy to implement and interpret.
Mang lang is similar to the [Json](https://www.json.org/) data format,
but with a minimal set of programming primitives added:
* References to entries in dictionaries
* If-then-else conditionals
* First class functions

Mang lang is a purely functional and interpreted language.
It takes source code written in Mang lang and evaluates it:  
```HiveQL
{
  rectangles = (
    {width = 3, height = 1},
    {width = 6, height = 2},
    {width = 3, height = 6},
    {width = 8, height = 4}
  ),
  get_area = from {width, height} to mul(width, height),
  areas = map(get_area, rectangles),
  total_area = add areas,
  num_rectangles = count rectangles,
  average_area = div(total_area, num_rectangles)
}
```
When we evaluate the source code above we get the result below:  
```HiveQL
{
  rectangles = (
    {width = 3, height = 1},
    {width = 6, height = 2},
    {width = 3, height = 6},
    {width = 8, height = 4}
  ),
  get_area = from {width, height} to mul(width, height),
  areas = (3, 12, 18, 32),
  total_area = 65,
  num_rectangles = 4,
  average_area = 16.25
}
```
Note that both the input and output of the mang lang interpreter is given as mang lang source code!

For more code examples continue reading below, or look inside the
[standard library](https://github.com/mabur/mang_lang/blob/master/mang_lang_cpp/built_in_functions/standard_library.h).

# Design Trade-offs

Mang lang has the following **design trade-offs**:

1. **Minimalistic**, instead of feature-rich.
2. **Prinicipled**, instead of pragmatic.
2. **Purely functional**, instead of imperative or object oriented.
3. **Eager evaluation**, instead of lazy evaluation.
4. **Dynamically typed and interpreted**, instead of statically typed and compiled.
5. **Code as Data and Data as Code**. The built-in data structures (lists and dictionaries) are also used to structure the code itself into multiple expressions, lines and variables. Furthermore, both the input data and output data of the Mang Lang interpretor is expressed in Mang Lang itself!

# Language Comparison

The minimalism of Manglang makes it resemble a simple data format like Json or Yaml.
Manglang only adds conditionals and functions to allow computations.

|                       | Json | Yaml | Manglang | C    |
| :-------------------- | :--- | :--- | :------- | :--- |
| Numbers               | Yes  | Yes  | Yes      | Yes  |
| Strings               | Yes  | Yes  | Yes      | Yes  |
| Lists/Arrays          | Yes  | Yes  | Yes      | Yes  |
| Dictionaries/Structs  | Yes  | Yes  | Yes      | Yes  |
| Null                  | Yes  | Yes  | -        | Yes  |
| Enum                  | -    | -    | -        | Yes  |
| Comments              | -    | Yes  | -        | Yes  |
| Aliases               | -    | Yes  | Yes      | Yes  |
| Pointers              | -    | -    | -        | Yes  |
| Mutable Variables     | -    | -    | -        | Yes  |
| Types                 | -    | -    | -        | Yes  |
| Conditionals          | -    | -    | Yes      | Yes  |
| Loops                 | -    | -    | -        | Yes  |
| Functions             | -    | -    | Yes      | Yes  |
| Operators             | -    | -    | -        | Yes  |
| Macros                | -    | -    | -        | Yes  | 

Mang lang is similar to these languages:
* [Json](https://www.json.org/)
* [Yaml](https://yaml.org/)
* [Jsonnet](https://jsonnet.org/)
* [Dhall](https://dhall-lang.org/)
* [L1](https://mlajtos.github.io/L1/)
* [Azor](https://github.com/cstuartroe/azor/)
* [Lisp Family](https://en.wikipedia.org/wiki/Lisp_(programming_language))

# Grammar

Manglang has a minimal syntax. A program/expression is built up from these building blocks:
 
| Kind of Expression    | Syntax                                        |
| :-------------------- | :-------------------------------------------- |
|number                 | 12.34                                         |
|character              | 'a'                                           |
|string                 | "abc"                                         |
|list                   | (expression, ...)                             |
|dictionary             | {name = expression, ...}                      |
|reference              | name                                          |
|child reference        | name@expression                               |
|conditional            | if expression then expression else expression |
|function               | from name to expression                       |
|function of list       | from (name, ...) to expression                |
|function of dictionary | from {name, ...} to expression                |
|function call          | name expression                               |

# Examples

1. Data
    1. [Numbers](#1i-numbers)
    2. [Characters](#1ii-characters)
    3. [Strings](#1iii-strings)
    4. [Lists](#1iv-lists)
    5. [Dictionaries](#1v-dictionaries)
2. References
    1. [Name lookup](#2i-name-lookup)
    2. [Child name lookup](#2ii-child-name-lookup)
3. Computation
    1. [Conditionals](#3i-conditionals)
    2. [Function calls](#3ii-function-calls)
    3. [Function definitions](#3iii-function-definitions)
    4. [Function of list definitions](#3iv-function-of-list-definitions)
    5. [Function of dictionary definitions](#3iv-function-of-dictionary-definitions)
 

## 1.I Numbers
Mang lang has a single number type that is used for both integers and floats:
```HiveQL
12.34
```

## 1.II Characters
A single ascii character is written as:
```HiveQL
'a'
```

## 1.III Strings
Strings are written as:
```HiveQL
"Mang lang"
```
They can be seen as lists of characters. Example of a program using functions on strings:
```HiveQL
{
a = "Mang lang",
b = first a,
c = rest a,
d = reverse a,
e = prepend('E', a)
}
```
This program is evaluated to:
```HiveQL
{
a = "Mang lang",
b = 'M',
c = "ang lang",
d = "gnal gnaM",
d = "EMang lang",
}
``` 

## 1.IV Lists
Lists of values are written as:
```HiveQL
(3, 6, 4)
```
Example of a program using functions on lists:
```HiveQL
{
a = (3, 6, 4),
b = first a,
c = rest a,
d = reverse a,
e = prepend(9, a) 
}
```
This program is evaluated to:
```HiveQL
{
a = (3, 6, 4),
b = 3,
c = (6, 4),
d = (4, 6, 3),
e = (9, 4, 6, 3),
}
``` 

## 1.V Dictionaries

Dictionaries are used to associate names/symbols with expressions:
```HiveQL
{a = 1, b = 'A', c = "abc"}
```
Mang lang doesn't care about whitespace so the program above can also be written as:
```HiveQL
{
a = 1,
b = 'A',
c = "abc"
}
```
Dictionaries can be nested:
```HiveQL
{
rectangle = {width = 4, height = 5},
circle = {radius = 5}
}
```
In Mang lang dictionaries are the only way to associate names/symbols with expressions.
So Mang lang uses dictionaries to represent both: variables, objects, function input, function output.
This is a beautiful generalization and simplification.

## 2.I Name Lookup

A name/symbol defined in a dictionary can be referenced after it is defined:
```HiveQL
{
a = 1,
b = a
}
```
This program is evaluated to the dictionary:
```HiveQL
{
a = 1,
b = 1
}
```
Dictionaries can be nested. You can refer to symbols in the current dictionary or in parent dictionaries in this way:
```HiveQL
{
a = 1,
b = {c = 2, d = a}
}
```
This program is evaluated to:
```HiveQL
{
a = 1,
b = {c = 2, d = 1}
}
```

## 2.II Child Name Lookup

In the previous section we looked at how to refer to names defined in the current dictionary, or in a parent dictionary.
You can also refer to names in a child dictionary like this:
```HiveQL
{
a = {b=2, c=3},
d = c@a
}
```
This program is evaluated to:
```HiveQL
{
a = {b=2, c=3},
d = 3
}
```
The syntax `name@dictionary` is used to get the value corresponding to the name/key inside the dictionary.
This syntax is reversed compared to most languages that instead write this as `dictionary.name`.
However, having it like this simplifies the syntax of Mangalng and makes it easier to parse.
It also makes both function application and dictionary lookup follow the same order and pattern. 

## 3.I Conditionals

A conditional is written as `if a then b else c` and this expression is evaluated to b or c depending of if a is true or false.
Mang lang has no explicit type for boolean values but interprets other values as true or false.
* Values that are interpreted as false:
  - the number zero `0`
  - the empty list `()`
  - the empty string `""` 
* Values that are interpreted as true:
  - all other numbers, lists and strings.

Consider this program as an example:

```HiveQL
{
a = (0, 1),
b = if a then
        first a
    else
        1,
c = if b then "hello" else "world"
}
```
This program is evaluated to:
```HiveQL
{
a = (0, 1),
b = 0,
c = "world"
}
```

## 3.II Function calls

We have already seen some examples of calling functions in mang lang.
A function is called like `function_name input_expression`.
Functions in take a single value as input.
However, this single value can be a list or a dictionary, that has multiple values inside them.
```HiveQL
{
list = (4, 2, 1),
sum0 = add list,
head0 = first list,
sum1 = add(4, 2, 1),
head1 = first(4, 2, 1),
list2 = prepend(3, list)
}
```
This program is evaluated to:
```HiveQL
{
list = (4, 2, 1),
sum0 = 7,
head0 = 4,
sum1 = 7,
head1 = 4,
list2 = (3, 4, 2, 1)
}
```
Mang Lang does not have any special operators for arithmetics, boolean, list operations etc.
Instead functions are used for all computations.
Function calls can be nested like this:

````HiveQL
mul(add(1, 2), sub(7, 2))
````

This program is evaluated to `(1+2)*(7-2) = 3*5 = 15`.


## 3.III Function Definitions

Functions are defined using they keywords `from` and `to` like this:

```HiveQL
{
square = from x to mul(x, x),
result = square 3
}
```

A function definition is on the form `from x to expression`
where `x` is the single input and expression is an expression using `x`.
Functions are first class values and can be given a name by putting them inside a dictionary.
Here are some examples of defining and calling functions:
```HiveQL
{
square = from x to mul(x, x),
inc = from x to add(x, 1),
dec = from x to sub(x, 1),
count = from list to if list then inc count rest list else 0,
a = square 3,
b = inc 3,
c = dec 3,
d = count(3,7,3,8,2),
e = count "apple"        
}
```
This program is evaluated to:
```HiveQL
{
square = from x to mul(x, x),
inc = from x to add(x, 1),
dec = from x to sub(x, 1),
count = from list to if list then inc count rest list else 0,
a = 9,
b = 4,
c = 2,
d = 5,
e = 5        
}
```
The if-then-else operator is used to choose what value to return based on a condition.
Recursive function calls and the if-then-else operator are used for loops:

```HiveQL
{
factorial = from x to 
    if x then
        mul(x, factorial dec x)
    else
        1,
result = factorial 4
}
```
Function definitions and computations can be broken up into smaller parts by using dictionaries:
```HiveQL
{
square = from x to mul(x, x)
square_norm = from vec3 to result@{
    x = first vec3,
    y = second vec3,
    z = third vec3,
    result = add(square x, square y, square z)
    }
vector = (3, 4, 5),
result = square_norm vector
}
```
## 3.IV Function of List Definitions

Mang lang provides syntactic sugar for defining functions that take multiple input,
in the form of a list.
Here are some examples of equivalent ways of defining and calling functions: 
```HiveQL
{
area1 = from rectangle to mul(first rectangle, second rectangle),
area2 = from (width, height) to mul(width, height),
rectangle = (5, 4),
a = area1 rectangle,
b = area2 rectangle,
c = area1(5, 4),
d = area2(5, 4),
}
```
The functions `area1` and `area2` are equivalent.
They expect the same input and return the same result.
`area2` just uses syntactic sugar to make it its implementation more clear,
by unpacking the elements of the input list.
## 3.V Function of Dictionary Definitions

Mang lang provides syntactic sugar for defining functions that take multiple input,
in the form of a dictionary with named entries.
Here are some examples of equivalent ways of defining and calling functions: 
```HiveQL
{
area1 = from rectangle to mul(width@rectangle, height@rectangle),
area2 = from {width, height} to mul(width, height),
rectangle = {width = 5, height = 4},
a = area1 rectangle,
b = area2 rectangle,
c = area1{width = 5, height = 4},
d = area2{width = 5, height = 4},
}
```
The functions `area1` and `area2` are equivalent.
They expect the same input and return the same result.
`area2` just uses syntactic sugar to make it its implementation more clear,
by unpacking the elements of the input dictionary.

## List of built-in functions

### Functions for numbers:
* **add**: adds a list of numbers.
* **mul**: multiplies a list of numbers.
* **sub**: subtracts two numbers.
* **div**: divides two numbers.


* **inc**: adds 1 to a number.
* **dec**: subtracts 1 from a number.


* **abs**: absolute value of a number.
* **sqrt**: square root of a number.


* **min**: smallest number in a non-empty list.
* **max**: largest number in a non-empty list.


* **increasing**: true if a list of numbers is increasing (<), and otherwise false.
* **decreasing**: true if a list of numbers is decreasing (>), and otherwise false.
* **weakly_increasing**: true if a list of numbers is weakly_increasing (<=), and otherwise false.
* **weakly_decreasing**: true if a list of numbers is weakly_decreasing (>=), and otherwise false.

### Logical functions:
* **equal**: true if two primitives are equal, and otherwise false.
* **unequal**: false if two primitives are equal, and otherwise true.
* **all**: true if all items of a list are true.
* **any**: true if at least one item of a list is true.
* **none**: true if all items of a list are false.


### List and string functions
* **prepend**: Given input `(item, list)` return a copy of the list/string with the item prepended at the beginning.
* **first**: pick the first item in a non-empty list/string.
* **rest**: list/string of all items except the first.
* **reverse**: takes a list/string and return it in reversed order.


* **map**: Given input `(f, list)` return a list where the function f has been applied to each item.
* **filter**: Given input `(predicate, list)` return a list of all items for which the predicate is true.
* **enumerate**: Given a list return a new list where each element is a dictionary `{item, index}` containing the items from the original list together with the corresponding index.
* **get_index**: Given input `(index, list)` return the item at given index.
* **concat**: concatenates two lists/strings.


* **count**: The number of items of a list or string.
* **count_item**: Given input `(item, list)` count number of occurrences of a specific item in list.
* **count_if**: Given input `(predicate, list)` count number of items in list for which the predicate is true.
