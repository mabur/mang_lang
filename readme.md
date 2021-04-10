# Mang Lang

Mang Lang is an experimental programming language. It explores how to make a language as minimalistic as possible, while still being useful. What are the essential building blocks needed for programming? How can we design a minimal language that is easy to learn and easy to interpret/compile. It has the following **design trade-offs**:

1. **Minimalistic**, instead of feature-rich.
2. **Prinicipled**, instead of pragmatic.
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
 
| Kind of Expression | Syntax                                        |
| :----------------- | :-------------------------------------------- |
|number              | 12.34                                         |
|character           | 'a'                                           |
|string              | "abc"                                         |
|list                | [expression, ...]                             |
|dictionary          | {name = expression, ...}                      |
|reference           | name                                          |
|child reference     | name<expression                               |
|conditional         | if expression then expression else expression |
|function            | from name to expression                       |
|function dictionary | from {name, ...} to expression                |
|function call       | name expression                               |

**Example code**:

```HiveQL
{
factorial = from x to 
    if x then
        mul[x, factorial dec x]
    else
        1,
result = factorial 4
}
```
This example program defines two symbols: `factorial` which is a function and `result` which is a number. Symbols are always defined within a dictionary `{factorial = ..., result = ...}`. White space and new lines are optional.

Functions are defined using the syntax `from ... to ...`, and called using the syntax `function input`.
Mang Lang has built in functions like `mul` and `dec` instead of having operators like `*` and `--`.
Functions take a single input and a single output. Multiple things are passed to and from functions by first putting them in either a list `[]` or a dictionary `{}`.

In the example above the result gets the value `1*2*3*4=24`.

Another example of how Mang Lang uses dictionaries both for structuring a program into multiple lines and creating objects/records:

```HiveQL
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

## Numbers
Mang lang has a single number type that is used for both integers and floats:
```HiveQL
12.34
```

## Characters
A single ascii character is written as:
```HiveQL
'a'
```

## Strings
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
e = prepend{first='E', rest=a}
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

## Lists
Lists of values are written as:
```HiveQL
[3, 6, 4]
```
Example of a program using functions on lists:
```HiveQL
{
a = [3, 6, 4],
b = first a,
c = rest a,
d = reverse a,
e = prepend{first=9, rest=a} 
}
```
This program is evaluated to:
```HiveQL
{
a = [3, 6, 4],
b = 3,
c = [6, 4],
d = [4, 6, 3],
e = [9, 4, 6, 3],
}
``` 

## Dictionaries

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

## Name Lookup

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

## Child Name Lookup

In the previous section we looked at how to refer to names defined in the current dictionary, or in a parent dictionary.
You can also refer to names in a child dictionary like this:
```HiveQL
{
a = {b=2, c=3},
d = c<a
}
```
This program is evaluated to:
```HiveQL
{
a = {b=2, c=3},
d = 3
}
```
The syntax `name<dictionary` is used to get the value corresponding to the name/key inside the dictionary.
We interpret `<` as an arrow that indicates that we get the name from the dictionary.

## Conditionals

A conditional is written as `if a then b else c` and this expression is evaluated to b or c depending of if a is true or false.
Mang lang has no explicit type for boolean values but interprets other values as true or false.
* Values that are interpreted as false:
  - the number zero `0`
  - the empty list `[]`
  - the empty string `""` 
* Values that are interpreted as true:
  - all other numbers, lists and strings.

Consider this program as an example:

```HiveQL
{
a = [0, 1],
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
a = [0, 1],
b = 0,
c = "world"
}
```

## Function calls

We have already seen some examples of calling functions in mang lang.
A function is called like `function_name input_expression`.
Functions in take a single value as input.
However, this single value can be a list or a dictionary, that has multiple values inside them.
```HiveQL
{
list = [4, 2, 1],
sum0 = add list,
head0 = first list,
sum1 = add[4, 2, 1],
head1 = first[4, 2, 1],
list2 = prepend{first=3, rest=list}
}
```
This program is evaluated to:
```HiveQL
{
list = [4, 2, 1],
sum0 = 7,
head0 = 4,
sum1 = 7,
head1 = 4,
list2 = [3, 4, 2, 1]
}
```
Mang Lang does not have any special operators for arithmetics, boolean, list operations etc.
Instead functions are used for all computations.
Function calls can be nested like this:

````HiveQL
mul[add[1, 2], sub[7, 2]]
````

This program is evaluated to `(1+2)*(7-2) = 3*5 = 15`.


## Function Definition

Functions are defined using they keywords `from` and `to` like this:

```HiveQL
{
square = from x to mul [x, x],
result = square 3
}
```

A function definition is on the form `from x to expression`
where `x` is the single input and expression is an expression using `x`.
Functions are first class values and can be given a name by putting them inside a dictionary.
Here are some examples of defining and calling functions:
```HiveQL
{
square = from x to mul[x, x],
inc = from x to add [x, 1],
dec = from x to sub [x, 1],
count = from list to if list then inc count rest list else 0,
a = square 3,
b = inc 3,
c = dec 3,
d = count [3,7,3,8,2],
e = count "apple"        
}
```
This program is evaluated to:
```HiveQL
{
square = from x to mul[x, x],
inc = from x to add [x, 1],
dec = from x to sub [x, 1],
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
        mul [x, factorial dec x]
    else
        1,
result = factorial 4
}
```
Function definitions and computations can be broken up into smaller parts by using dictionaries:
```HiveQL
{
square = from x to mul[x, x]
square_norm = from vec3 to result<{
    x = first vec3,
    y = second vec3,
    z = third vec3,
    result = add[square x, square y, square z]
    }
vector = [3, 4, 5],
result = square_norm vector
}
```
## Function Dictionary Definition

Mang lang provides syntactic sugar for defining functions that take multiple input,
in the form of a dictionary with named entries.
Here are some examples of equivalent ways of defining and calling functions: 
```HiveQL
{
area1 = from rectangle to mul [width<rectangle, height<rectangle],
area2 = from {width, height} to mul [width, height],
rectangle = {width = 5, height = 4},
a = area1 rectangle,
b = area2 rectangle,
c = area1 {width = 5, height = 4},
d = area2 {width = 5, height = 4},
}
```

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
* **append**: Given input `{list,item}` return a copy of the list/string with the item appended at the end.
* **first**: pick the first item in a non-empty list/string.
* **rest**: list/string of all items except the first.
* **reverse**: takes a list/string and return it in reversed order.


* **map**: Given input `{list, f}` return a list where the function f has been applied to each item.
* **filter**: Given input `{list, predicate}` return a list of all items for which the predicate is true.
* **enumerate**: Given a list return a new list where each element is a dictionary `{item, index}` containing the items from the original list together with the corresponding index.
* **split**: Given input `{list,separator}` split the list at the separators and return a list of sub lists.
* **get_index**: Given input `{list,index}` return the item at given index.
* **get_wrapped_index**: Given input `{list,index}` return the item at given index. If the index is outside the range of the list then it wraps around.
* **concat**: concatenates two lists/strings.


* **count**: The number of items of a list or string.
* **count_item**: Given input `{list, item}` count number of occurrences of a specific item in list.
* **count_if**: Given input `{list, predicate}` count number of items in list for which the predicate is true.
