#include "vm.h"

void run_program(virtual_machine* vm, program* p)
{
    call_info* c = vm->ci == -1 ? NULL : &vm->call_stack[vm->ci];

    call_info call = {
        .program = p,
        .pc = 0,
        .bp = c == NULL ? 0 : c->tp,
        .sp = c == NULL ? 0 : c->tp + p->symbol_table.size,
        .tp = c == NULL ? 0 : c->tp + p->symbol_table.size,
        .prev = c,
    };

    vm->call_stack[++vm->ci] = call;

    while (call.pc < p->length)
    {
        decode_execute(vm, call.program->code[call.pc]);
        
        if (call.program->code[call.pc].stackop.op == OP_RET) {
            break;
        }
        
        call.pc++;
    }

    vm->ci--;
}

void decode_execute(virtual_machine* vm, instruction i)
{
    call_info* call = &vm->call_stack[vm->ci];
    Value v0, v1;

    switch (i.stackop.op)
    {
        case OP_NOP: break;
        case OP_ADD:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int + v1.value.to_int);
            break;

        case OP_SUB:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int - v1.value.to_int);
            break;

        case OP_MUL:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int * v1.value.to_int);
            break;

        case OP_DIV:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int / v1.value.to_int);
            break;
        
        case OP_MOD:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = *vInt(v0.value.to_int % v1.value.to_int);
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
            program* code = vm->stack[--call->tp].value.to_code;
            call->tp -= code->argc;

            if (i.ux.ux == code->argc)
                run_program(vm, code);
            else
                failure("Invalid number of arguments passed to function!");
            break;
        
        case OP_RET:
            vm->stack[call->prev->tp++] = vm->stack[--call->tp];
            break;
        
        case OP_POP:
            call->tp--;
            break;
        
        default:
            fprintf(stderr, "%s Failed to decode instruction: %s\n", ERROR, disassemble(call->program, i));
            exit(0);
            break;
    }
}
