# Mang Lang

Mang Lang is a toy programming language with the following **design trade-offs**:

1. Simple instead of advanced.
2. Minimalistic instead of feature-rich.
3. Functional instead of imperative or object oriented.

**What it has**:

1. Numbers
2. Variables
3. Functions
4. Conditional expressions (if then else)
5. Tuples

**What it does NOT have**:

1. Control structures like: for, while, switch. If-then-else is the only control structure and recursion is used for loops.
2. Operators for arithmetic and boolean operations etc. Functions are used instead of operators.
3. Object oriented stuff like constructors, destructors, inheritance. Tuples are used for simple objects.
4. Mutable variables. Everything is immutable.

**Example code**:

```(
(
faculty(x) = if equal(x,0) then 1 else mul(x,faculty(sub(x,1))),
result = faculty(4)
)
```

This example first defines a function named `faculty`. It then defines a variable named `result` to be the value of `faculty(4)` i.e. `1*2*3*4=24`. Mang Lang uses functions like `equal`, `mul`, `sub` instead of having operators like `==`, `*` , `-`. Recursion is used for loops. Whitespace and new lines are optional.

# Examples

1. [Numbers and Built-in Functions](#numbers-and-built-in-functions)
2. [Variables and Multiple Statements](#variables-and-multiple-statements)
3. [Tuples](#tuples)
4. [Functions](#functions)
5. [Scope](#scope)

## Numbers and Built-in Functions

Mang Lang can be used as a calculator. This is a trivial program that just contains a single number:

```
8
```

This program is unsurprisingly evaluated to the value  `8`. Mang Lang does not have any operators for arithmetics etc, but instead use functions:

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

Functions are defined and used like this:

```
(
square_norm(x) = add(mul(x[0], x[0]), mul(x[1], x[1])),
vector = (3, 4),
result = square_norm(vector)
)
```

The if-then-else operator is used to choose what value to return based on a condition:

```
(
abs(x) = if greater(x, 0) then x else sub(0, x)
result = abs(-10)
)
```

Recursive function calls and the if-then-else operator are used for loops:

```
(
faculty(x) = if equal(x, 0) then 1 else mul(x, faculty(sub(x, 1))),
result = faculty(10)
)
```

## Scope

Function definitions can be broken up into smaller parts that are put in a local scope `{}`:

```
(
square_norm(v) = {
    x2 = mul(v[0], v[0]),
    y2 = mul(v[1], v[1])
    } = add(x2, y2),
vector = (3, 4),
result = square_norm(vector)
)
```

Scopes can be used for both function definitions and variable definitions:

```
margin = {top_margin = 5, bottom_margin = 10} = add(top_margin, bottom_margin)
```

