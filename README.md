# Mizantoro Programming Language

## General idea

I created this language because of my curiosity. I wanted to learn more about programming languages and how to write interpreters. Another reason as to why I wrote this interpreter was to create a language that uses mathematical terminology and Polish notation. For example instead of using `let` or `int` to create variables, my language uses `def` from "define". Another difference from most popular languages is its way of writing expressions and operations. Most used way of writing expressions is `a + b`. In Polish notation it is `+ a b` - the operation type comes first. The same rule applies to every other expression - the desired operation always comes before the variable name. I also wanted to make this language easy to read by avoiding using very simplified shortcuts for expressions. Keep in mind that this interpreter is provided as is and unexpected behavior may occur. I tested the language on a few most basic algorithms and got no errors, but always be careful.

You can see example programs I created in this [repository](https://github.com/Mizantoro/ml-example-programs)

## Syntax

### General Information

The language does not use parentheses or braces {}. Instead, blocks are defined using indentation and `end` statements.

In order to style the code use spaces only. Tabs are not accepted by the interpreter.

### Creating Comments

In order to create a comment `#` symbol must be used. It must be at the beginning of a line. It cannot go after any other expressions.

Example:

`# This is a working comment`

Example of wrong use:

`def num variable 0 # This is not a working comment`

### Variables and types

#### Declaring variables

In order to create a variable `def` expression must be used. Next expressions should stand for variable type. Available variable types are specified in the section below. Next expression should state the variable name. The last expression stands for variable's default value. Note that every variable must have a default value.

General syntax for declaring variables:

`def [type] [name] [value]`

Example of a variable declaration is shown below:

`def num variable 0` - This expression creates a numeric variable called "variable" with the default value of 0.

#### Types

This language supports three variable types:

* `num` - numeric
* `str` - string
* `bool` - boolean

Note that string variable type cannot be modified during program execution and is used only to simplify creating output expressions.

#### Assigning Values

In order to assign a value to a variable `set` expression is required. Next expression should state the name of the target variable. Next expression is operation type. The operation type must match the target variable and value types. The two following values can be either a variable name or a value expressed in decimal.

The general syntax of this expression is:

`set [target] [operation] [value 1] [value 2]`

Examples of assigning a value:

`set variable add 1 3` - variable will hold the sum of 1 and 3.

`set variable div variable n` - variable will hold the result of being divided by variable n.

If you want to copy a value from one variable to another do this:

`set variable add other_variable 0`

## Expressions and Operators

### Math Operators

Available math operators:

* `add` - Addition
* `sub` - Subtraction
* `mul` - Multiplication
* `div` - Division
* `pow` - Power

Every operator can be expressed by either a word or a character:

* `add` or `+`
* `sub` or `-`
* `mul` or `*`
* `div` or `/`
* `pow` or `^`

### Comparison Operators

The language contains five comparison types:

* `equals` - a equals b
* `greater` - a is greater than b
* `greater_equals` - a is greater or equal to b
* `lesser` - a is lesser than b
* `lesser_equals` - a is lesser or equal to b

Every operator can be expressed by either a word or characters:

* `equals` or `==`
* `greater` or `>`
* `greater_equals` or `>=`
* `lesser` or `<`
* `lesser_equals` or `<=`

## Flow Control

### If Statements

If statement returns a boolean value. If the value is true then code below if statement will be executed, until reaching `end` expression. The language supports `else` expression. `else` must follow `end` of the previous `if` statement and must also end with `end`.

General syntax for if statements:

```
if [operator] [value 1] [value 2]
  #code to be executed
end
```

Example:

```
if equals variable 0
  #code
end
else
  #code
end
```

### While Loops

While loops work similarly to if statements, but will repeat the code below them until the condition is no longer true. To learn how to create conditions go to section "If Statements".

General syntax for while loops:

```
while [operator] [value 1] [value 2]
  #code to be executed
end
```

Example:

```
while equals variable 0
  #code
end
```

### End

`end` expressions mark the end of a nestable statement (for example if or while).

### Kill

`kill` expression will immediately stop the execution of a program.

## Input / Output

### Input

In order to get a value from user `input` expression must be used.

General syntax for input:

`input [variable]`

Example:

`input n` - will get value from user and save it to variable n.

### Output

In order to display a variable or a string `display` expression must be used.

Display accepts three types:

* `var` - displays a variable
* `str` - displays text specified later
* `endl` - creates a new line

General syntax:
`display [type] [variable / value]`

Case for `var`:

`display var [variable]`

Case for `str`:

`display str [text]`

Case for `endl`:

`display endl`

Examples:

`display var variable` - displays a variable named "variable"

`display str This is example text` - displays string "This is example text"

`display endl` - creates a new line

## Examples of code:

### Checking if number is prime:

```
def num n 0
display str Insert n:
input n
if lesser_equals n 1
  display str False
  kill
end
def num i 2
def num tmp 0
while lesser i n
  set tmp mod n i
  if equals tmp 0
    display str False
    kill
  end
  set i add i 1
end
display str True
```

```
def num n 0
display str Insert n:
input n
if <= n 1
  display str False
  kill
end
def num i 2
def num tmp 0
while < i n
  set tmp % n i
  if == tmp 0
    display str False
    kill
  end
  set i + i 1
end
display str True
```
