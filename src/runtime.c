#include "interpreter.h"

TValue apply_binary_operation(vm_op op, TValue v0, TValue v1);

void execute_program(interpreter* in)
{
    call_info c = {
        .bp = 0,
        .sp = 0,
        .pc = 0,
        .size = in->program.size,
        .program = in->program.instructions,
        .prev = NULL
    };

    // sets base call
    in->vm.calls[0] = c;
    in->vm.ccall = &c;

    for (; in->vm.ccall->pc < in->program.size; in->vm.ccall->pc++)
    {
        execute_instruction(in, in->vm.ccall->program[in->vm.ccall->pc]);
    }
}

void execute_instruction(interpreter* in, uint32_t instruction)
{
    vm_op op = instruction & 0xff;
    uint8_t r0 = (instruction >> 8) & 0xff;
    uint8_t r1 = (instruction >> 16) & 0xff;
    uint8_t r2 = (instruction >> 24) & 0xff;
    uint8_t ix = (instruction >> 16) & 0xffff;

    switch (op)
    {
        case OP_LDK:
            in->vm.top++;
            in->vm.registers[in->vm.ccall->sp + r0] = in->program.constants[ix];
            break;

        case OP_LDG:
            in->vm.top++;
            in->vm.registers[in->vm.ccall->sp + r0] = in->vm.heap[ix];
            break;

        case OP_STG:
            in->vm.top--;
            in->vm.heap[ix] = in->vm.registers[in->vm.ccall->sp + r0];
            break;
        
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            TValue v0 = in->vm.registers[in->vm.ccall->sp + r1];
            TValue v1 = in->vm.registers[in->vm.ccall->sp + r2];
            in->vm.registers[in->vm.ccall->sp + r0] = apply_binary_operation(op, v0, v1);
            in->vm.top--; // TODO: Based on assumption!!
            break;

        default:
            runtime_err("Failed to decode statement!");
    }
}

TValue apply_binary_operation(vm_op op, TValue v0, TValue v1)
{
    switch (op)
    {
        case OP_ADD:
            return *TInt(v0.value.i + v1.value.i);
        case OP_SUB:
            return *TInt(v0.value.i - v1.value.i);
        case OP_MUL:
            return *TInt(v0.value.i * v1.value.i);
        case OP_DIV:
            return *TInt(v0.value.i / v1.value.i);
    
        default:
            runtime_err("Binary operation not supported!");
            return *TNull();
    }
}

void runtime_err(const char* msg)
{
    fprintf(stderr, "%s %s\n", ERROR, msg);
    exit(0);
}