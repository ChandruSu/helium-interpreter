#ifndef HE_EXECUTION_H
#define HE_EXECUTION_H

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

Value value_from_node(astnode* node);

const char* value_to_str(Value* v);

Value* vNull();
Value* vInt(int i);
Value* vFloat(float f);
Value* vString(const char* s);
Value* vBool(boolean b);
Value* vCode(program* p);

// ------------------- VM IR --------------------

typedef enum vm_op {
    OP_ADD,             // 0
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,             // 4
    OP_PUSHK,
    OP_STORG,
    OP_LOADG,
    OP_STORL,           // 8
    OP_LOADL,
    OP_CALL,
} vm_op;

typedef enum vm_scope {
    VM_LOCAL_SCOPE,
    VM_GLOBAL_SCOPE,
    VM_CLOSED_SCOPE,
    VM_UNKNOWN_SCOPE,
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
    Value* constants;
    struct program* prev;
    const char* src_code;

    map symbol_table;
    map constant_table;
} program;

void compile(program* p, astnode* root);

void compile_statement(program* p, astnode* statement);

void compile_assignment(program* p, astnode* s);

void compile_expression(program* p, astnode* expression);

void compile_call(program* p, astnode* call);

void compile_function(program* p, astnode* function);

void compilererr(program* p, lxpos pos, const char* msg);

uint16_t register_constant(program* p, Value v);

int16_t register_variable(program* p, const char* name, vm_scope* scope);

int16_t dereference_variable(program* p, const char* name, vm_scope* scope);

const char* disassemble(program* p, instruction i);

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