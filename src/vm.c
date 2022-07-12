#include "vm.h"

void run_program(virtual_machine* vm, call_info* prev, code_object* code)
{
    size_t ci = ++vm->ci;

    vm->call_stack[ci].program = code;
    vm->call_stack[ci].pc = 0;
    vm->call_stack[ci].bp = prev == NULL ? 0 : prev->tp;
    vm->call_stack[ci].sp = prev == NULL ? 0 : prev->tp + code->p->symbol_table.size;
    vm->call_stack[ci].tp = prev == NULL ? 0 : prev->tp + code->p->symbol_table.size;
    vm->call_stack[ci].prev = prev;

    call_info* call = &vm->call_stack[ci];

    if (vm->ci >= MAX_CALL_STACK) {
        runtimeerr(vm, "Function call limit reached!");
    } else if (call->tp >= MAX_STACK_SIZE) {
        runtimeerr(vm, "Stack overflow!");
    }

    if (code->p->native != NULL) 
    {
        vm->stack[call->tp++] = code->p->native(&vm->stack[call->bp]);
        vm->stack[call->prev->tp++] = vm->stack[--call->tp];
    }

    while (call->pc < code->p->length)
    {
        decode_execute(vm, call, code->p->code[call->pc]);
        
        if (code->p->code[call->pc].stackop.op == OP_RET) {
            break;
        }
        
        call->pc++;
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
            vm->stack[call->tp - 1] = vNegate(vm->stack[call->tp - 1]);
            break;

        case OP_NOT:
            vm->stack[call->tp - 1] = vBool(!native_bool_cast(&vm->stack[call->tp - 1]).value.to_bool);
            break;

        case OP_PUSHK:
            vm->stack[call->tp++] = call->program->p->constants[i.ux.ux];
            
            if (call->tp >= MAX_STACK_SIZE) runtimeerr(vm, "Stack overflow!");
            break;

        case OP_STORG:
            vm->heap[i.sx.sx] = vm->stack[--call->tp];

            if (i.sx.sx >= MAX_HEAP_SIZE) runtimeerr(vm, "Stack overflow!");
            break;

        case OP_LOADG:
            vm->stack[call->tp++] = vm->heap[i.sx.sx];

            if (call->tp >= MAX_STACK_SIZE) runtimeerr(vm, "Stack overflow!");
            break;
        
        case OP_STORL:
            vm->stack[call->bp + i.sx.sx] = vm->stack[--call->tp];

            if (call->bp + i.sx.sx >= MAX_STACK_SIZE) runtimeerr(vm, "Stack overflow!");
            break;
        
        case OP_LOADL:
            vm->stack[call->tp++] = vm->stack[call->bp + i.sx.sx];

            if (call->tp >= MAX_STACK_SIZE) runtimeerr(vm, "Stack overflow!");
            break;

        case OP_LOADC:
            vm->stack[call->tp++] = call->program->closure[i.ux.ux];

            if (call->tp >= MAX_STACK_SIZE) runtimeerr(vm, "Stack overflow!");
            break;

        case OP_CALL:
            if (vm->stack[--call->tp].type != VM_PROGRAM) {
                char msg[1000];
                msg[0] = '\0';
                sprintf(msg, "Cannot call value %s, expected function type!", value_to_str(&vm->stack[--call->tp]));
                runtimeerr(vm, msg);
            }

            code_object* code = vm->stack[call->tp].value.to_code;

            call->tp -= code->p->argc;

            if (i.ux.ux == code->p->argc)
                run_program(vm, call, code);
            else {
                runtimeerr(vm, "Invalid number of arguments passed to function!");
            }
            break;
        
        case OP_RET:
            vm->stack[call->prev->tp++] = vm->stack[--call->tp];
            break;
        
        case OP_POP:
            call->tp--;
            break;
        
        case OP_JIF:
            if (native_bool_cast(&vm->stack[--call->tp]).value.to_bool) {
                call->pc++;
            }
            break;

        case OP_JMP:
            call->pc += i.sx.sx;
            break;
        
        case OP_CLOSE:
            Value* closure = malloc(sizeof(Value) * i.ux.ux);
            call->tp -= i.ux.ux;
            
            for (size_t i0 = 0; i0 < i.ux.ux; i0++) {
                closure[i0] = vm->stack[call->tp + i0];
            }

            vm->stack[call->tp - 1] = vCode(vm->stack[call->tp - 1].value.to_code->p, closure);
            break;
        
        default:
            fprintf(stderr, "%s Failed to execute instruction: %s\n", ERROR, disassemble(call->program->p, i));
            exit(0);
            break;
    }
}

Value apply_vm_op(vm_op op, Value v0, Value v1)
{
    switch (op)
    {
        case OP_ADD: return vAdd(v0, v1);
        case OP_SUB: return vSub(v0, v1);
        case OP_MUL: return vMul(v0, v1);
        case OP_DIV: return vDiv(v0, v1);
        case OP_MOD: return vMod(v0, v1);
        case OP_EQ: return vEqual(v0, v1);
        case OP_NE: return vNotEqual(v0, v1);
        case OP_LE: return vLessEqual(v0, v1);
        case OP_LT: return vLess(v0, v1);
        case OP_GE: return vLessEqual(v1, v0);
        case OP_GT: return vLess(v1, v0);
        
        case OP_AND: return vBool(native_bool_cast(&v0).value.to_bool && native_bool_cast(&v1).value.to_bool);
        case OP_OR: return vBool(native_bool_cast(&v0).value.to_bool || native_bool_cast(&v1).value.to_bool);
        default:
            fprintf(stderr, "%s Failed to apply binary operation: %i!\n", ERROR, op);
            exit(0);
    }
}

void runtimeerr(virtual_machine* vm, const char* msg)
{
    fprintf(stderr, "%sError Stack Trace: \n", ERR_COL);
    
    // prints stack trace.
    for (size_t i = 0; i <= vm->ci; i++)
    {
        call_info call = vm->call_stack[i];

        // native methods have no instructions
        if (call.program->p->native) {
            continue;
        }

        lxpos* pos = getaddresspos(call.program->p, call.pc);
        Value v = vCode(call.program->p, NULL);
        fprintf(stderr, "\t%s In file %s at line %i:\n", value_to_str(&v), pos->origin, pos->line_pos + 1);
        fprintf(stderr, "\t\t| %04i %s\n", pos->line_pos + 1, get_line(pos->src, pos->line_offset));
    }

    fprintf(stderr, "Runtime error: %s%s\n", msg, DEF_COL);
    exit(0);
}