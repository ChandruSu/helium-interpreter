#include "vm.h"

Value apply_vm_op(vm_op op, Value v0, Value v1);

void run_program(virtual_machine* vm, call_info* prev, program* p)
{
    call_info call = {
        .program = p,
        .pc = 0,
        .bp = prev == NULL ? 0 : prev->tp,
        .sp = prev == NULL ? 0 : prev->tp + p->symbol_table.size,
        .tp = prev == NULL ? 0 : prev->tp + p->symbol_table.size,
        .prev = prev,
    };

    vm->call_stack[++vm->ci] = call;

    if (vm->ci >= MAX_CALL_STACK) {
        failure("Function call limit reached!");
    } else if (call.tp >= MAX_STACK_SIZE) {
        failure("Stack overflow!");
    }

    if (call.program->native != NULL) 
    {
        vm->stack[call.tp++] = call.program->native(vm->stack[call.bp]);
        vm->stack[call.prev->tp++] = vm->stack[--call.tp];
    }

    while (call.pc < p->length)
    {
        decode_execute(vm, &call, call.program->code[call.pc]);
        
        if (call.program->code[call.pc].stackop.op == OP_RET) {
            break;
        }
        
        call.pc++;
    }

    vm->ci--;
}

void decode_execute(virtual_machine* vm, call_info* call, instruction i)
{
    Value v0, v1;

    switch (i.stackop.op)
    {
        case OP_NOP: break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_AND:
        case OP_OR:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_EQ:
        case OP_NE:
            v1 = vm->stack[--call->tp];
            v0 = vm->stack[--call->tp];
            vm->stack[call->tp++] = apply_vm_op(i.stackop.op, v0, v1);
            break;

        case OP_NEG:
            vm->stack[call->tp - 1] = vInt(-vm->stack[call->tp - 1].value.to_int);
            break;

        case OP_NOT:
            vm->stack[call->tp - 1] = vBool(!vm->stack[call->tp - 1].value.to_bool);
            break;

        case OP_PUSHK:
            vm->stack[call->tp++] = call->program->constants[i.ux.ux];
            
            if (call->tp >= MAX_STACK_SIZE) failure("Stack overflow!");
            break;

        case OP_STORG:
            vm->heap[i.sx.sx] = vm->stack[--call->tp];

            if (i.sx.sx >= MAX_HEAP_SIZE) failure("Stack overflow!");
            break;

        case OP_LOADG:
            vm->stack[call->tp++] = vm->heap[i.sx.sx];

            if (call->tp >= MAX_STACK_SIZE) failure("Stack overflow!");
            break;
        
        case OP_STORL:
            vm->stack[call->bp + i.sx.sx] = vm->stack[--call->tp];

            if (call->bp + i.sx.sx >= MAX_STACK_SIZE) failure("Stack overflow!");
            break;
        
        case OP_LOADL:
            vm->stack[call->tp++] = vm->stack[call->bp + i.sx.sx];

            if (call->tp >= MAX_STACK_SIZE) failure("Stack overflow!");
            break;

        case OP_CALL:
            program* code = vm->stack[--call->tp].value.to_code;
            call->tp -= code->argc;

            if (i.ux.ux == code->argc)
                run_program(vm, call, code);
            else
                failure("Invalid number of arguments passed to function!");
            break;
        
        case OP_RET:
            vm->stack[call->prev->tp++] = vm->stack[--call->tp];
            break;
        
        case OP_POP:
            call->tp--;
            break;
        
        case OP_JIF:
            if (vm->stack[--call->tp].value.to_bool) {
                call->pc++;
            }
            break;

        case OP_JMP:
            call->pc += i.sx.sx;
            break;
        
        default:
            fprintf(stderr, "%s Failed to execute instruction: %s\n", ERROR, disassemble(call->program, i));
            exit(0);
            break;
    }
}

Value apply_vm_op(vm_op op, Value v0, Value v1)
{
    switch (op)
    {
        case OP_ADD: return vInt(v0.value.to_int + v1.value.to_int);
        case OP_SUB: return vInt(v0.value.to_int - v1.value.to_int);
        case OP_MUL: return vInt(v0.value.to_int * v1.value.to_int);
        case OP_DIV: return vInt(v0.value.to_int / v1.value.to_int);
        case OP_MOD: return vInt(v0.value.to_int % v1.value.to_int);
        case OP_AND: return vBool(v0.value.to_int && v1.value.to_int);
        case OP_OR: return vBool(v0.value.to_int || v1.value.to_int);
        case OP_EQ: return vBool(v0.value.to_int == v1.value.to_int);
        case OP_NE: return vBool(v0.value.to_int != v1.value.to_int);
        case OP_LE: return vBool(v0.value.to_int <= v1.value.to_int);
        case OP_GE: return vBool(v0.value.to_int >= v1.value.to_int);
        case OP_LT: return vBool(v0.value.to_int < v1.value.to_int);
        case OP_GT: return vBool(v0.value.to_int > v1.value.to_int);
        default:
            fprintf(stderr, "%s Failed to apply binary operation: %i!\n", ERROR, op);
            exit(0);
    }
}
