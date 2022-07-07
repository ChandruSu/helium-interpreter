#ifndef HE_EXECUTION_HEADER
#define HE_EXECUTION_HEADER

#include <stdint.h>
#include "common.h"
#include "datatypes.h"
#include "parser.h"

// ------------------ VM TYPES ------------------

typedef struct program program;

typedef enum vm_type {
    VM_NULL,
    VM_INT,
    VM_BOOL,
    VM_FLOAT,
    VM_STRING,
    VM_PROGRAM,
} vm_type;

typedef struct Value {
    vm_type type;
    union {
        boolean to_bool;
        int32_t to_int;
        float to_float;
        const char* to_str;
        program* to_code;
    } value;
} Value;

/**
 * @brief Coerces abstract syntax node into Value object.
 * 
 * @param node Abstract syntax node
 * @return Value
 */
Value value_from_node(astnode* node);

/**
 * @brief Represents VM Value object as a string.
 * 
 * @param v Value
 * @return string
 */
const char* value_to_str(Value* v);

/**
 * @brief Constructor for null value.
 * 
 * @return Pointer to null Value
 */
Value* vNull();

/**
 * @brief Constructor for int value.
 * 
 * @return Pointer to int Value
 */
Value* vInt(int i);

/**
 * @brief Constructor for float value.
 * 
 * @return Pointer to float Value
 */
Value* vFloat(float f);

/**
 * @brief Constructor for string value.
 * 
 * @return Pointer to string Value
 */
Value* vString(const char* s);

/**
 * @brief Constructor for bool value.
 * 
 * @return Pointer to bool Value
 */
Value* vBool(boolean b);

/**
 * @brief Constructor for code object value.
 * 
 * @return Pointer to code Value
 */
Value* vCode(program* p);

// ------------------- VM IR --------------------

typedef enum vm_op {
    OP_NOP,             // 0
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,             // 4
    OP_NEG,
    OP_PUSHK,
    OP_STORG,
    OP_LOADG,           // 8
    OP_STORL,
    OP_LOADL,
    OP_CALL,
    OP_RET,             // 12
    OP_POP,
} vm_op;

typedef enum vm_scope {
    VM_LOCAL_SCOPE,
    VM_GLOBAL_SCOPE,
    VM_CLOSED_SCOPE,
    VM_UNKNOWN_SCOPE,
    VM_DUPLICATE_IN_SCOPE,
} vm_scope;

typedef union instruction {
    struct {
        vm_op op;
    } stackop;
    
    struct {
        vm_op op;
        uint8_t r0;
        uint8_t r1;
        uint8_t r2;
    } r0r1r2;

    struct {
        vm_op op;
        uint16_t ux;
    } ux;
    
    struct {
        vm_op op;
        int16_t sx;
    } sx;

    uint32_t bits;
} instruction;

typedef struct program {
    instruction* code;
    size_t length;
    size_t argc;
    Value* constants;
    struct program* prev;
    const char* src_code;

    map symbol_table;
    map constant_table;
} program;

/**
 * @brief Compiles block of statements into bytecode and stores
 *      it into program.
 * 
 * @param p Reference to program
 * @param block Statement block node
 */
void compile(program* p, astnode* block);

/**
 * @brief Determines nature of statement and compiles it into
 *      bytecode. Can compile: assignments, function definitions,
 *      function calls, return statements.
 * 
 * @param p Reference to program
 * @param statement Statement node
 */
void compile_statement(program* p, astnode* statement);

/**
 * @brief Compiles variable-value assignment into byte code and
 *      stores in program.
 * 
 * @param p Reference to program
 * @param s Assignment statement node
 */
void compile_assignment(program* p, astnode* s);

/**
 * @brief Compiles expression nodes into byte code - including:
 *      variable references, number literals, function calls,
 *      unary and binary expressions.
 * 
 * @param p Reference to program
 * @param expression Expression node
 */
void compile_expression(program* p, astnode* expression);

/**
 * @brief Compiles function call and argument expressions into 
 *      bytecode.
 * 
 * @param p Reference to program 
 * @param call Function call node
 */
void compile_call(program* p, astnode* call);

/**
 * @brief Compiles function definition, creates a new program
 *      object and stores function body in the new program. The
 *      program will be stored as a constant in local scope. 
 * 
 * @param p Reference to program
 * @param function Function definition node
 */
void compile_function(program* p, astnode* function);

/**
 * @brief Prints error message into standard error output and
 *      displays location in source raising the error.
 * 
 * @param p Reference to program
 * @param pos Position of error in source
 * @param msg Error message
 */
void compilererr(program* p, lxpos pos, const char* msg);

/**
 * @brief Registers constant value in local scope and stores value
 *      string in program's constant table. Method returns the address
 *      of constant (index) in constant stack.
 * 
 * @param p Reference to program
 * @param v Value to register
 * @return Address
 */
uint16_t register_constant(program* p, Value v);

/**
 * @brief Registers variable symbol and returns the address within
 *      stack or heap. Stores scope of variable in scope pointer. If
 *      variable is already registered, address is returned.
 * 
 * @param p Reference to program
 * @param name Name to register
 * @param scope Scope output
 * @return Address
 */
int16_t register_variable(program* p, const char* name, vm_scope* scope);

/**
 * @brief Registers new variable symbol in local scope and returns the
 *      address. If symbol already exist, output scope pointer is set to
 *      VM_DUPLICATE_IN_SCOPE to indicate duplicate symbol declaration.
 * 
 * @param p Reference to program
 * @param name Name to register
 * @param scope Scope output
 * @return Address
 */
int16_t register_unique_variable_local(program* p, const char* name, vm_scope* scope);

/**
 * @brief Retrieves the address of a variable in the stack 
 *      (relative to stack pointer) or in heap memory. Methods 
 *      also stores the scope of the variable reference to vm_scope 
 *      pointer.
 * 
 * @param p Reference to program
 * @param name Name of variable
 * @param scope Scope output
 * @return Address
 */
int16_t dereference_variable(program* p, const char* name, vm_scope* scope);

/**
 * @brief Decodes entire program into a string and also decodes 
 *      any programs stored in program constants.
 * 
 * @param p Reference to program
 * @return String
 */
const char* disassemble_program(program* p);

/**
 * @brief Decodes bytecode instruction into a string and
 *      dereferences variables and constants.
 * 
 * @param p Reference to program
 * @param i Instruction
 * @return String
 */
const char* disassemble(program* p, instruction i);

#endif