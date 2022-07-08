The Helium Interpreter
---

A lightweight, fast interpreter written in C for 'Helium script', a custom esoteric programming language.

The helium language is dynamically typed, follows a procedural programming paradigm and functions are treated as first class objects.

## Building

GNU build-tools are required to build the interpreter, the following binaries are required:
+ make
+ gcc
+ gdb (for debugging)

To build the interpreter, download the source code and run the following commands in sequence:

```bash
cd ./helium-interpreter
mkdir out
mkdir bin
make all
```

The interpreter executable can be found in the `out/` directory.

## Language Syntax

1. Variable assignments
    
    ```c
    foo <- 10 * bar + 7
    ```

2. If, else if, else blocks

    ```c
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
    loop (a < 100) {
        a <- a + 1
    }
    ```

4. Comments

    ```c
    # this is a oneline comment
    
    ? This is 
    a multiline 
    comment ?
    ```

5. Function declarations

    ```c
    bar <- $(x, y) {
        return 5 * x * y
    }
    ```
    The helium interpreter treats functions as first-class objects which can be stored in variables and passed as arguments


6. Function calls

    ```c
    @foo(2)
    baz <- @bar(7, 3)

    factorial <- $(n) {
        if n > 0 {
            return n * @factorial(n - 1)
        } else {
            return 1
        }
    }
    ```
    Function calls must be preceeded with the `@` character. Recursive function calls are allowed

## Features

### Implemented

+ Bools, String literals
+ Operator order precedence
+ Integer arithmetic
+ Boolean operations
+ First class functions
+ Global/local scoping
+ Bytecode disassembly
+ Error source printing
+ While loops

### Upcoming

+ Floating point arithmetic
+ Function closures
+ Control structures
    - If statements
+ Complex data types
+ Garbage collection
+ Bitwise operations
