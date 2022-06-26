#ifndef HE_INTERPRETER_HEADER
#define HE_INTERPRETER_HEADER

#include "parser.h"
#include "datatypes.h"

#define MAX_CALLS 0xff
#define MAX_REGISTERS 0xffff
#define MAX_CONSTANTS 0xffff
#define MAX_HEAP_SIZE 0xffff

typedef enum vm_op {
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
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
    TValue* constants;
    TValue* registers;
    TValue* heap;
} virtual_machine;

typedef struct interpreter {
    unsigned int* program;
    size_t program_size;
    virtual_machine vm;
} interpreter;

void interpret(interpreter* in, astnode* n);

void interpret_expression(interpreter* in, astnode* n);

unsigned int encode_instruction_3R(vm_op op, short r0, short r1, short r2);

#endif