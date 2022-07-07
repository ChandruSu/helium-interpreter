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
    foo <- a * 10 + 7
    ```

2. Function declarations

    ```c
    bar <- $(x, y) {
        return 5 * x * y
    }
    ```
    The helium interpreter treats functions as first-class objects which can be stored in variables and passed as arguments


3. Function calls

    ```c
    @foo(1, 2)
    baz <- @bar(7)
    ```
    Function calls must be preceeded with the `@` character

## Features
### Implemented
+ Operator order precedence
+ Integer arithmetic
+ First class functions
+ Global/local scoping
+ Bytecode disassembly
+ Error source printing

### Upcoming
+ Function closures
+ Control structures
    - If statements
    - While loops
+ Bools, Floats, Strings
+ Complex data types
+ Garbage collection
