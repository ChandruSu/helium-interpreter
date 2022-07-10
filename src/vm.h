#ifndef HE_VM_HEADER
#define HE_VM_HEADER

#include <inttypes.h>
#include "compiler.h"
#include "lib.h"

// --------------------- VM ---------------------

typedef struct call_info {
    code_object* program;
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

/**
 * @brief Runs program or code object by pushing new call info
 *      to call stack and simulating stack frame on vm stack.
 * 
 * @param vm Reference to virtual machine
 * @param prev Previous call information
 * @param code Code value containing program to run
 */
void run_program(virtual_machine* vm, call_info* prev, code_object* code);

/**
 * @brief Decodes instruction and executes it using the current
 *      call information and virtual machine context.
 * 
 * @param vm Reference to virtual machine
 * @param i Instruction to decode and execute
 */
void decode_execute(virtual_machine* vm, call_info* call, instruction i);

void runtimeerr(virtual_machine* vm, const char* msg);

#endif