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

To execute a helium script file:
1. Install the binary to your system or have the executable in the same directory as the target file
2. Run the following command on your terminal:

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
    ```
    Function calls must be preceded with the `@` character. Recursive function calls are allowed and function closures are also possible

    ```c++
    # function closure
    multiplier <- $(n) { return $(x) { return x * n } }
    mul2 <- @multiplier(2)

    @print(@mul2(5)) # prints 10
    ```
    When a function definition references a variable in a parent method, this method will be dereferenced and stored in a **Closure** object linked with the function instance

6. Comments

    ```c++
    # this is a single line comment
    
    ? This is an example of
    a multiline comment ?
    ```

7. In-built functions, string concatenation and character extraction

    ```c++
    a <- @float(@input("Enter a number: "))
    @print("Square root of " + @str(a) + " is " + @str(@sqrt(a)))

    # retrieves fifth character "o"
    char5 <- "Hello World!" % 4
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

8. Table data structure

    ```c++
    table <- { "key1": 1, "key2": null, 3: false, null: 1.07 }
    table["key3"] <- true
    @print(table["key2"]) # prints null
    ```
    Currently, the sole complex data structure available is the **Table** which maps keys to values. Square brackets can be used to retrieve values by key or insert values with keys. The keys and values for each entry in the table do not need to be of the same type. As of now, there is no way to remove entries from the table
    
    ```c++
    # prints key2
    @print(table % 1)
    ```

    The modulus operation can be used to fetch any key in the table by index
    
    ```c++
    table.func <- $(n) { return n + 1 }
    @table.func(4)
    ```
    The dot operator `.` can be used as a shorthand for table retrieval but only works with keys that follow symbol format (alphanumeric, underscores, doesn't begin with a numeric). This allows functions to be stored and called directly to and from a table

9. Importing other files

    ```c++
    include "relative/path/to/file.he"

    @methodFromFile()
    ```
    Include statements can only be executed within global scope. File paths should be given relative to the current script file. Avoid using circular or duplicate imports

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
+ Table data structure

### Upcoming

+ Optimising compiler
+ Garbage collection
+ Bitwise operations
+ File IO
