#ifndef HE_VM_HEADER
#define HE_VM_HEADER

#include "common.h"
#include "compiler.h"
#include "lib.h"

// --------------------- VM ---------------------

typedef struct call_info {
    code_object* program;
    size_t bp;
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

/**
 * @brief Applies a virtual machine operation between two generic tagged
 *      values.
 * 
 * @param op Operation code
 * @param v0 Operand 1
 * @param v1 Operand 2
 * @return Result value
 */
Value apply_vm_op(vm_op op, Value v0, Value v1);

/**
 * @brief Throws a runtime error when an issue occurs during
 *      bytecode execution. Stack trace is used to determine the
 *      source of the error.
 * 
 * @param vm Reference to virtual machine
 * @param msg Error message
 */
void runtimeerr(virtual_machine* vm, const char* msg);

#endif