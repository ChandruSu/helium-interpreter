#ifndef HE_VM_HEADER
#define HE_VM_HEADER

#include <inttypes.h>
#include "compiler.h"

// --------------------- VM ---------------------

typedef struct call_info {
    program* program;
    size_t bp;              // registers
    size_t sp;
    size_t tp;
    size_t pc;
    struct call_info* prev;
} call_info;

typedef struct virtual_machine {
    size_t ci;
    call_info* call_stack;
    Value* heap;
    Value* stack;
} virtual_machine;

void run_program(virtual_machine* vm, program* p);

void decode_execute(virtual_machine* vm, instruction i);

#endif