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

void translate_ast(program* p, astnode* n);

void translate_statement(program* p, translator* t, astnode* n);

void translate_expression(program* p, translator* t, astnode* n);

unsigned int encode_instruction_3R(vm_op op, short r0, short r1, short r2);

unsigned int encode_instruction_R_i(vm_op op, short r0, short i);

unsigned short register_constant(program* p, translator* t, astnode* k);

unsigned short register_global(program* p, translator* t, astnode* g);

unsigned short load_global(program* p, translator* t, astnode* g);

void translate_err(program* p, astnode* node, const char* msg);

#endif