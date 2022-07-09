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
    loop a < 100 {
        a <- a + 1
    }
    ```

4. Function declarations

    ```c
    bar <- $(x, y) {
        return 5 * x * y
    }
    ```
    The helium interpreter treats functions as first-class objects which can be stored in variables and passed as arguments


5. Function calls

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

6. Comments

    ```c
    # this is a oneline comment
    
    ? This is 
    a multiline 
    comment ?
    ```

## Features

### Upcoming

+ Floating point arithmetic
+ Function closures
+ Complex data types
+ Garbage collection
+ Bitwise operations
