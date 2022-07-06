#include "vm.h"

void run_program(virtual_machine* vm, program* p)
{
    call_info* c = vm->ci == -1 ? NULL : &vm->call_stack[vm->ci++];

    call_info call = {
        .program = p,
        .pc = 0,
        .bp = c == NULL ? 0 : c->tp,
        .sp = c == NULL ? 0 : c->tp + p->symbol_table.size,
        .tp = c == NULL ? 0 : c->tp + p->symbol_table.size,
        .prev = c,
    };

    vm->call_stack[vm->ci] = call;

    while (call.pc < p->length)
    {
        decode_execute(vm, call.program->code[call.pc]);
        call.pc++;
    }
}

void decode_execute(virtual_machine* vm, instruction i)
{
    call_info* call = &vm->call_stack[vm->ci];
    Value v0, v1;

    switch (i.stackop.op)
    {
        case OP_NOP: break;
        case OP_ADD:
            v0 = vm->stack[--call->tp];
            v1 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int + v1.value.to_int);
            break;

        case OP_SUB:
            v0 = vm->stack[--call->tp];
            v1 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int - v1.value.to_int);
            break;

        case OP_MUL:
            v0 = vm->stack[--call->tp];
            v1 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int * v1.value.to_int);
            break;

        case OP_DIV:
            v0 = vm->stack[--call->tp];
            v1 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int / v1.value.to_int);
            break;

        case OP_NEG:
            vm->stack[call->tp - 1] = *vInt(-vm->stack[call->tp - 1].value.to_int);
            break;

        case OP_PUSHK:
            vm->stack[call->tp++] = call->program->constants[i.ux.ux];
            break;

        case OP_STORG:
            vm->heap[i.sx.sx] = vm->stack[--call->tp];
            break;

        case OP_LOADG:
            vm->stack[call->tp++] = vm->heap[i.sx.sx];
            break;
        
        case OP_STORL: 
            vm->stack[call->bp + i.sx.sx] = vm->stack[--call->tp];
            break;
        
        case OP_LOADL: 
            vm->stack[call->tp++] = vm->stack[call->bp + i.sx.sx];
            break;

        case OP_CALL: 
            break;
        
        case OP_RET: 
            break;
        
        default:
            fprintf(stderr, "%s Failed to decode instruction: %s\n", ERROR, disassemble(vm->call_stack[vm->ci].program, i));
            exit(0);
            break;
    }
}
