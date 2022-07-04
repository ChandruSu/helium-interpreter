#ifndef HE_EXECUTION_H
#define HE_EXECUTION_H

#include <stdint.h>
#include "common.h"
#include "datatypes.h"
#include "parser.h"

// ------------------ VM TYPES ------------------

typedef enum vm_type {
    VM_INT,
    VM_BOOL,
    VM_FLOAT,
    VM_STRING,
    VM_POINTER,
} vm_type;

typedef struct Value {
    vm_type type;
    union {
        boolean to_bool;
        int32_t to_int;
        float to_float;
        const char* to_str;
        void* to_pointer;
    } value;
} Value;

// ------------------- VM IR --------------------

typedef enum vm_op {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOADK,
} vm_op;

typedef union instruction {
    struct {
        vm_op op;
        uint8_t r0;
        uint8_t r1;
        uint8_t r2;
    } r0r1r2;

    struct {
        vm_op op;
        uint8_t r0;
        uint16_t ux;
    } r0ux;
    
    struct {
        vm_op op;
        uint8_t r0;
        int16_t sx;
    } r0sx;

    uint32_t bits;
} instruction;

typedef struct program {
    instruction* code;
    size_t length;
    Value* constants;
    struct program* prev;
    const char* src_code;
} program;

void compile(program* p, astnode* root);

void compile_statement(program* p, astnode* statement);

void compile_assignment(program* p, astnode* s);

void compile_expression(program* p, astnode* expression);

void compilererr(program* p, lxpos pos, const char* msg);

// --------------------- VM ---------------------

typedef struct vm {
    size_t bp;
    size_t sp;
    size_t tp;
    size_t ci;
    vector call_stack;
    Value* heap;
    Value* stack;
} vm;


#endif