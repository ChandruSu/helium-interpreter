#ifndef HE_INTERPRETER_HEADER
#define HE_INTERPRETER_HEADER

#include "parser.h"
#include "datatypes.h"
#include "stdlib.h"
#include "stdio.h"
#include "inttypes.h"

#define MAX_CALLS 0xff
#define MAX_REGISTERS 0xffff
#define MAX_CONSTANTS 0xffff
#define MAX_HEAP_SIZE 0xffff

typedef enum vm_op {
    OP_MOV,     // 0
    OP_LDK,
    OP_LDG,
    OP_STG,
    OP_LDL,     // 4
    OP_STL,
    OP_LDC,
    OP_ADD,
    OP_SUB,     // 8
    OP_MUL,
    OP_DIV,
    OP_NEG,
} vm_op;

typedef enum vm_scope {
    SCOPE_NONE,
    SCOPE_LOCAL,
    SCOPE_CLOSED,
    SCOPE_GLOBAL
} vm_scope;

typedef struct call_info {
    size_t bp;
    size_t sp;
    size_t pc;
    size_t size;
    uint32_t* program;
    struct call_info* prev;
} call_info;

typedef struct virtual_machine {
    size_t top;
    call_info* ccall;
    call_info* calls;
    TValue* registers;
    TValue* heap;
} virtual_machine;

typedef struct translator {
    size_t sp;
    map constant_table;
    map symbol_table;
} translator;

typedef struct program {
    uint32_t* instructions;
    size_t size;
    TValue* constants;
    const char* source;
    translator translator;
    struct program* prev;
} program;

typedef struct interpreter {
    program program;
    virtual_machine vm;
} interpreter;

// ------------------------ TRANSLATION ------------------------

/**
 * @brief Translates whole abstract syntax tree and stores intermediate
 *      assembly into program struct.
 * 
 * @param p Reference to program
 * @param n Root node
 */
void translate_ast(program* p, astnode* n);

/**
 * @brief Translates statement node into intermediate assembly code.
 * 
 * @param p Reference to program
 * @param n Statement node
 */
void translate_statement(program* p, astnode* n);

/**
 * @brief Translates expression node into intermediate assembly and
 *      recursively resolves binary expressions.
 * 
 * @param p Reference to program
 * @param n Expression node
 */
void translate_expression(program* p, astnode* n);

uint16_t translate_function_definition(program* p, astnode* f);

/**
 * @brief Registers constant literal if it does not already exist or
 *      returns its address within the constant table.
 * 
 * @param p Reference to program
 * @param cname Constant name
 * @param val Constant value
 * @return address in constant table
 */
unsigned short register_constant(program* p, const char* cname, TValue val);

uint16_t register_variable(program* p, const char* vname, vm_scope* scope);

uint16_t dereference_variable(program* p, const char* vname, vm_scope* scope);

/**
 * @brief Produces 32-bit instruction that can be executed. Register addresses
 *      are relative to the stack pointer.
 * 
 * @param op Op-code
 * @param r0 Register index 1
 * @param r1 Register index 2
 * @param r2 Register index 3
 * @return 32-bit binary instruction 
 */
uint32_t encode_instruction_3R(vm_op op, uint8_t r0, uint8_t r1, uint8_t r2);

/**
 * @brief Produces 32-bit instruction that can be executed. Register addresses
 *      are relative to the stack pointer.
 * 
 * @param op Op-code
 * @param r0 Register index 1
 * @param i 16-bit operand
 * @return 32-bit binary instruction 
 */
uint32_t encode_instruction_R_i(vm_op op, uint8_t r0, uint16_t i0);

/**
 * @brief Prints semantic error and displays location of error in source code.
 * 
 * @param p Reference to program
 * @param node Node that caused error
 * @param msg Error message
 */
void translate_err(program* p, astnode* node, const char* msg);

void disassemble_program(program* p);

/**
 * @brief Disassembles binary instruction and converts it to string representation.
 * 
 * @param p Reference to program
 * @param ins 32-bit instruction
 * @return String
 */
const char* disassemble_instruction(program* p, uint32_t ins);

// ------------------------- EXECUTION -------------------------

void execute_program(interpreter* in);

void execute_instruction(interpreter* in, uint32_t instruction);

void runtime_err(const char* msg);

#endif