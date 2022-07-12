# The Helium Interpreter

A lightweight, fast interpreter written in C for 'Helium script', a custom esoteric programming language.

The helium language is dynamically typed, follows a procedural programming paradigm and functions are treated as first class objects.

## Building

GNU build-tools are required to build the interpreter, the following binaries are required:
+ make
+ gcc

To build the interpreter, download the source code and run the following commands in sequence:

```bash
cd ./helium-interpreter
mkdir out
mkdir bin
make all
```

The interpreter executable can be found in the `out/` directory.

## Installing & Running

To execute a helium script file, first install the binary to your system or have the executable in the same directory as the target file, then run the following command on your terminal:

```bash
helium filename.he
```

Use the demo scripts in the `demo/` directory to test the interpreter.

## Language Syntax

1. Variable assignments
    
    ```c++
    foo <- 10 * bar + 7
    ```

2. If, else if, else blocks

    ```c++
    if a && b {
        # block 1
    } else if c || d {
        # block 2
    } else if !e {
        # block 3
    } else {
        # block 4
    }
    ```

3. While loops

    ```rust
    loop a < 100 {
        a <- a + 1
    }
    ```

4. Function declarations

    ```c++
    bar <- $(x, y) {
        return 5 * x * y
    }
    ```
    The helium interpreter treats functions as first-class objects which can be stored in variables and passed as arguments


5. Function calls

    ```c++
    @foo(2)
    baz <- @bar(7, 3)

    # recursive function
    factorial <- $(n) {
        if n > 0 {
            return n * @factorial(n - 1)
        } else {
            return 1
        }
    }

    # function closure
    multiplier <- $(n) { return $(x) { return x * n } }
    mul2 <- @multiplier(2)
    ```
    Function calls must be preceeded with the `@` character. Recursive function calls are allowed and function closures are also possible

6. Comments

    ```c++
    # this is a oneline comment
    
    ? This is 
    a multiline 
    comment ?
    ```

7. In-built functions and string concatenation

    ```c++
    a <- @float(@input("Enter a number: "))
    @print("Square root of " + @str(a) + " is " + @str(@sqrt(a)))
    ```

    Available methods:
    + **print** - writes message string to standard output
    + **input** - reads line from standard input and returns string
    + **int** - casts value to integer value
    + **str** - casts value to string value
    + **float** - casts value to floating point value
    + **bool** - casts value to boolean value
    + **len** - returns the length of a string
    + **pow** - calculates *x* raised to the power of *y*
    + **sqrt** - calculates the square root of a *x*
    + **time** - returns the current time since the program began in milliseconds
    + **delay** - sleeps the program by *x* millseconds

8. Importing other files

    ```c++
    include "relative/path/to/file.he"

    @methodFromFile()
    ```
    Include statements can only be executed at global scope. Filepaths are relative to the current script file.

## Features

### Implemented

+ Primitive types:
    - Integers
    - Floating points
    - Booleans
    - Strings
+ Iteration, branching, functions
+ Functions are first class objects
+ Function closures
+ Lambda functions (anonymous methods)
+ Recursion
+ File imports

### Upcoming

+ Complex data types
+ Garbage collection
+ Bitwise operations
+ File IO
