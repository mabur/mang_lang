# Mang Lang

Mang Lang is an experimental programming language. It explores how to make a language as minimalistic as possible, while still being useful. What are the essential building blocks needed for programming? How can we design a minimal language that is easy to learn and easy to interpret/compile. It has the following **design trade-offs**:

1. **Minimalistic**, instead of feature-rich.
2. **Purely functional**, instead of imperative or object oriented.
3. **Eager evaluation**, instead of lazy evaluation.
4. **Dynamically typed and interpreted**. Static typing and compiling might be explored eventually.
5. **Code as Data and Data as Code**. The built-in data structures (lists and dictionaries) are also used to structure the code itself into multiple expressions, lines and variables. Furthermore, both the input data and output data of the Mang Lang interpretor is expressed in Mang Lang itself!
   
The motivation behind Mang Lang is somewhat similar to Lisp, but it has evolved its own unique solutions to a similar problem statement. Mang Lang could be described as a **love child of Json and Lisp**.

**Language Comparison**

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


**Grammar**

Manglang has a minimal syntax. A program/expression is built up from these building blocks:
 
| Kind of Expression | Syntax                                       |
| :----------------- | :-------------------------------------------- |
|number              | 12.34                                         |
|string              | "abc"                                         |
|list                | [expression, ...]                             |
|dictionary          | {name = expression, ...}                      |
|reference           | name                                          |
|child reference     | name<expression                               |
|conditional         | if expression then expression else expression |
|function            | from name to expression                       |
|function call       | name expression                               |

**Example code**:

```(
{
factorial = from x to 
    if equal [x, 0] then
        1
    else
        mul [x, factorial sub [x, 1]],
result = factorial 4
}
```
This example program defines two symbols: `factorial` which is a function and `result` which is a number. Symbols are always defined within a dictionary `{factorial = ..., result = ...}`. White space and new lines are optional.

Functions are defined using the syntax `from ... to ...`, and called using the syntax `function input`.
Mang Lang has built in functions like `equal`, `mul`, `sub` instead of having operators like `==`, `*` , `-`. Functions take a single input and a single output. Multiple things are passed to and from functions by first putting them in either a list `[]` or a dictionary `{}`.

In the example above the result gets the value `1*2*3*4=24`.

Another example of how Mang Lang uses dictionaries both for structuring a program into multiple lines and creating objects/records:

```
{
rectangle = {width = 4, height = 5},
area = mul [width<rectangle, height<rectangle]
}
```

In this example area gets the value `4*5=20`.
We use the syntax `width<rectangle` to get the field `width` from the dictionary `rectangle`.

# Examples

1. [Numbers and Built-in Functions](#numbers-and-built-in-functions)
2. [Dictionaries and Variables](#dictionaries-and-variables)
3. [Lists](#lists)
4. [Functions and Control Flow](#functions-and-control-flow)
5. [Strings](#Strings)
6. [Importing code in different source files](#importing-code-in-different-source-files)
7. [List of built-in functions](#list-of-built-in-functions)
8. [List of functions in standard library](#list-of-functions-in-standard-library) 

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
area = mul [width<rectangle, height<rectangle]
}
```
We use the syntax `width<rectangle` to get the field `width` from the dictionary `rectangle`.

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
s = count dubble_list
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
    if equal [x, 0] then
        1
    else
        mul [x, factorial sub [x, 1]],
result = factorial 4
}
```
Function definitions and computations can be broken up into smaller parts by using dictionaries:

```
{
square_norm = from vec2 to result<{
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
tau    = mul [2, pi<math],
square = square<math,
four   = square 2
}
```

## List of built-in functions

### Functions for numbers:
* **add**: adds a list of numbers.
* **mul**: multiplies a list of numbers.
* **sub**: subtracts two numbers.
* **div**: divides two numbers.


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
* **is_empty**: true if a list/string has zero items.
* **append**: Given input `{list,item}` return a copy of the list/string with the item appended at the end.
* **concat**: concatenates two lists/strings.
* **first**: pick the first item in a non-empty list/string.
* **last**: pick the last item in a non-empty list/string.
* **first_part**: list/string of all items except the last.
* **last_part**: list/string of all items except the first.


## List of functions in standard library

* **map**: Given input `{list, f}` return a list where the function f has been applied to each item.
* **filter**: Given input `{list, predicate}` return a list of all items for which the predicate is true.
* **enumerate**: Given a list return a new list where each element is a dictionary `{item, index}` containing the items from the original list together with the corresponding index.
* **reverse**: flip the order of a list.
* **split**: Given input `{list,separator}` split the list at the separators and return a list of sub lists.
* **get_index**: Given input `{list,index}` return the item at given index.
* **get_wrapped_index**: Given input `{list,index}` return the item at given index. If the index is outside the range of the list then it wraps around. 


* **count**: The number of items of a list or string.
* **count_item**: Given input `{list, item}` count number of occurrences of a specific item in list.
* **count_if**: Given input `{list, predicate}` count number of items in list for which the predicate is true.


* **find**: Given input `{list, item}` find the first occurrence of a specific item in the list.
  Returns two lists. The list up until but excluding the item, and the remaining list from the item and to the end.  
* **find_if**: Given input `{list, predicate}` find the first item in the list for which the predicate is true.
  Returns two lists. The list up until but excluding the item, and the remaining list from the item and to the end.
 