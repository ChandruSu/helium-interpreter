#ifndef HE_INTERPRETER_HEADER
#define HE_INTERPRETER_HEADER

#include "parser.h"
#include "datatypes.h"

#define MAX_CALLS 0xff
#define MAX_REGISTERS 0xffff
#define MAX_CONSTANTS 0xffff
#define MAX_HEAP_SIZE 0xffff

typedef enum vm_op {
    OP_MOV,     // 0
    OP_LDK,
    OP_LDG,
    OP_STG,
    OP_ADD,     // 4
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,     // 8
} vm_op;

typedef struct call_info {
    size_t bp;
    size_t sp;
    size_t pc;
    struct call_info* prev;
} call_info;

typedef struct virtual_machine {
    call_info* calls;
    size_t ci;
    TValue* registers;
    TValue* heap;
} virtual_machine;

typedef struct program {
    unsigned int* instructions;
    size_t size;
    TValue* constants;
    const char* source;
} program;

typedef struct interpreter {
    program program;
    virtual_machine vm;
} interpreter;

typedef struct translator {
    size_t sp;
    map constant_table;
    map symbol_table;
} translator;

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
 * @param t Translator object
 * @param n Statement node
 */
void translate_statement(program* p, translator* t, astnode* n);

/**
 * @brief Translates expression node into intermediate assembly and
 *      recursively resolves binary expressions.
 * 
 * @param p Reference to program
 * @param t Translator object
 * @param n Expression node
 */
void translate_expression(program* p, translator* t, astnode* n);

/**
 * @brief Registers constant literal if it does not already exist or
 *      returns its address within the constant table.
 * 
 * @param p Reference to program
 * @param t Translator object
 * @param k Constant node
 * @return address in constant table
 */
unsigned short register_constant(program* p, translator* t, astnode* k);

/**
 * @brief Registers variable symbol if it does not already exist in the
 *      global scope. Returns the current symbol address if variable does
 *      exist.
 * 
 * @param p Reference to program
 * @param t Translator object
 * @param g Symbol node
 * @return address in symbol table
 */
unsigned short register_global(program* p, translator* t, astnode* g);

/**
 * @brief Fetches global variable address or throws an error if variable
 *      does not exist.
 * 
 * @param p Reference to program
 * @param t Translator object
 * @param g Symbol node
 * @return address in symbol table
 */
unsigned short load_global(program* p, translator* t, astnode* g);

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
unsigned int encode_instruction_3R(vm_op op, short r0, short r1, short r2);

/**
 * @brief Produces 32-bit instruction that can be executed. Register addresses
 *      are relative to the stack pointer.
 * 
 * @param op Op-code
 * @param r0 Register index 1
 * @param i 16-bit operand
 * @return 32-bit binary instruction 
 */
unsigned int encode_instruction_R_i(vm_op op, short r0, short i0);

/**
 * @brief Prints semantic error and displays location of error in source code.
 * 
 * @param p Reference to program
 * @param node Node that caused error
 * @param msg Error message
 */
void translate_err(program* p, astnode* node, const char* msg);

#endif