#include "exec.h"

vm_op decode_op(const char* operator);

vm_op scope_load_op_map[] = {
    OP_LOADL,
    OP_LOADG,
    OP_NOP,
};

vm_op scope_store_op_map[] = {
    OP_STORL,
    OP_STORG,
    OP_NOP,
};

// -------------- COMPILER METHODS --------------

void compile(program* p, astnode* block)
{
    for (size_t i = 0; i < block->children.size; i++)
    {
        compile_statement(p, vector_get(&block->children, i));
    }
}

void compile_statement(program* p, astnode* statement)
{
    switch (statement->type)
    {
        case AST_ASSIGN:
            compile_assignment(p, statement);
            break;
        
        case AST_CALL:
            compile_call(p, statement);
            break;
        
        case AST_RETURN:
            if (p->prev == NULL) {
                compilererr(p, statement->pos, "Cannot use return statement in global scope!");
            }

            compile_expression(p, vector_get(&statement->children, 0));
            p->code[p->length++].stackop.op = OP_RET;
            break;
        
        default:
            compilererr(p, statement->pos, "Failed to compile statement into bytecode!");
    }
}

void compile_assignment(program* p, astnode* s)
{
    vm_scope scope;
    astnode* rhs = vector_get(&s->children, 0);

    if (rhs->type == AST_FUNCTION)
        compile_function(p, rhs);
    else
        compile_expression(p, rhs);

    p->code[p->length].sx.sx = register_variable(p, s->value, &scope);
    p->code[p->length].sx.op = scope_store_op_map[scope];
    p->length++;
}

void compile_call(program* p, astnode* call)
{
    for (size_t i = 0; i < call->children.size; i++)
    {
        compile_expression(p, vector_get(&call->children, i));   
    }
    
    vm_scope scope;
    p->code[p->length].sx.sx = dereference_variable(p, call->value, &scope);
    p->code[p->length].sx.op = scope_load_op_map[scope];
    p->length++;

    if (scope == VM_UNKNOWN_SCOPE) {
        compilererr(p, call->pos, "Unknown function name!");
    }

    p->code[p->length++].stackop.op = OP_CALL;
}

void compile_function(program* p, astnode* function)
{
    program* p0 = malloc(sizeof(program));
    p0->code = malloc(sizeof(instruction) * 0xff);
    p0->length = 0;
    p0->constants = malloc(sizeof(Value) * 0xff);
    p0->src_code = p->src_code;
    p0->prev = p;
    p0->constant_table = map_new(37);
    p0->symbol_table = map_new(37);

    // register parameter names
    astnode* params = vector_get(&function->children, 0);
    for (p0->argc = 0; p0->argc < params->children.size; p0->argc++)
    {
        vm_scope scope;
        astnode* param = vector_get(&params->children, p0->argc);
        register_unique_variable_local(p0, param->value, &scope);  
    
        if (scope == VM_DUPLICATE_IN_SCOPE) {
            compilererr(p0, param->pos, "Duplicate variable name in function definition!");
        }
    }

    // compiles program code
    compile(p0, vector_get(&function->children, 1));;

    // stores code object as local constant
    p->code[p->length].ux.op = OP_PUSHK;
    p->code[p->length].ux.ux = register_constant(p, *vCode(p0));
    p->length++;
}

void compile_expression(program* p, astnode* expression)
{
    switch (expression->type)
    {
        case AST_BINARY_EXPRESSION:
            compile_expression(p, vector_get(&expression->children, 0));
            compile_expression(p, vector_get(&expression->children, 1));
            p->code[p->length++].stackop.op = decode_op(expression->value);
            break;
        
        case AST_UNARY_EXPRESSION:
            compile_expression(p, vector_get(&expression->children, 0));

            if (streq(expression->value, "-")) {
                p->code[p->length++].stackop.op = OP_NEG;
            }
            break;

        case AST_CALL:
            compile_call(p, expression);
            break;
        
        case AST_REFERENCE:
            vm_scope scope;
            p->code[p->length].sx.sx = dereference_variable(p, expression->value, &scope);
            p->code[p->length].sx.op = scope_load_op_map[scope];
            p->length++;

            if (scope == VM_UNKNOWN_SCOPE)
                compilererr(p, expression->pos, "Unknown variable name!");
            break;

        case AST_INTEGER:
            p->code[p->length].ux.op = OP_PUSHK;
            p->code[p->length].ux.ux = register_constant(p, value_from_node(expression));
            p->length++;
            break;

        default:
            compilererr(p, expression->pos, "Failed to compile expression!");
    }
}

// ---------------- MEMORY STORE ----------------

uint16_t register_constant(program* p, Value v)
{
    Value* address;

    if ((address = map_get(&p->constant_table, value_to_str(&v))) == NULL) 
    {
        address = vInt(p->constant_table.size);
        map_put(&p->constant_table, value_to_str(&v), address);
        p->constants[address->value.to_int] = v;
    }
    
    return address->value.to_int;
}

int16_t register_variable(program* p, const char* name, vm_scope* scope)
{
    size_t address = dereference_variable(p, name, scope);

    if (*scope == VM_UNKNOWN_SCOPE) {
        Value* a = vInt(p->symbol_table.size);
        map_put(&p->symbol_table, name, a);
        *scope = p->prev == NULL ? VM_GLOBAL_SCOPE : VM_LOCAL_SCOPE;
        return a->value.to_int;
    } else {
        return address;
    }
}

int16_t register_unique_variable_local(program* p, const char* name, vm_scope* scope)
{
    Value* address = map_get(&p->symbol_table, name);

    if (address == NULL) {
        address = vInt(p->symbol_table.size);
        map_put(&p->symbol_table, name, address);
        *scope = p->prev == NULL ? VM_GLOBAL_SCOPE : VM_LOCAL_SCOPE;
        return address->value.to_int;
    } else {
        *scope = VM_DUPLICATE_IN_SCOPE;
        return address->value.to_int;
    }
}

int16_t dereference_variable(program* p, const char* name, vm_scope* scope)
{
    Value* address = map_get(&p->symbol_table, name);
    program* p0 = p;

    // iteratively determines variable location
    while (address == NULL && p0->prev != NULL) {
        p0 = p0->prev;
        address = map_get(&p0->symbol_table, name);
    }

    // determines scope of variable
    if (address == NULL) {
        *scope = VM_UNKNOWN_SCOPE;
        return 0;
    } else if (p0->prev == NULL) {
        *scope = VM_GLOBAL_SCOPE;
    } else if (p0 == p) {
        *scope = VM_LOCAL_SCOPE;
    } else {
        *scope = VM_CLOSED_SCOPE;
    }
    
    return address->value.to_int;
}

// ------------------- UTILS --------------------

vm_op decode_op(const char* operator)
{
    if (streq(operator, "+"))
        return OP_ADD;
    else if (streq(operator, "-"))
        return OP_SUB;
    else if (streq(operator, "*"))
        return OP_MUL;
    else if (streq(operator, "/"))
        return OP_DIV;
    else
        failure("Failed to decode operator!");
    return 0;
}

const char* operation_strings[] = {
    "OP_NOP      ",             // 0
    "OP_ADD      ",
    "OP_SUB      ",
    "OP_MUL      ",
    "OP_DIV      ",             // 4
    "OP_NEG      ",
    "OP_PUSHK    ",
    "OP_STORG    ",
    "OP_LOADG    ",             // 8
    "OP_STORL    ",
    "OP_LOADL    ",
    "OP_CALL     ",
    "OP_RET      ",             // 12
};

const char* disassemble_program(program* p) 
{
    char* buf = malloc(sizeof(char) * 32 * 100);
    buf[0] = '\0';

    // disassembles instructions
    for (size_t i = 0; i < p->length; i++)
    {
        strcat(buf, "\t");
        strcat(buf, disassemble(p, p->code[i]));
        strcat(buf, "\n");
    }

    for (size_t i = 0; i < p->constant_table.size; i++)
    {
        Value* index = p->constant_table.values[i];
        Value program = p->constants[index->value.to_int];

        if (program.type == VM_PROGRAM)
        {
            strcat(buf, "\n");
            strcat(buf, value_to_str(&program));
            strcat(buf, ":\n");
            strcat(buf, disassemble_program(p->constants[index->value.to_int].value.to_code));
        }
    }
    
    return buf;
}

const char* disassemble(program* p, instruction i) {
    char* buf = malloc(sizeof(char) * 32);

    switch (i.stackop.op)
    {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_NEG:
        case OP_CALL:
        case OP_RET:
        case OP_NOP:
            sprintf(buf, "%s", operation_strings[i.stackop.op]);
            break;
        
        case OP_PUSHK:
            Value c = p->constants[i.ux.ux];
            sprintf(buf, "%s %u (%s)", operation_strings[i.stackop.op], i.ux.ux, value_to_str(&c));
            break;
        
        case OP_STORG:
        case OP_LOADG:
            // uses global program to decode symbols
            while (p->prev != NULL) p = p->prev;
        case OP_STORL:
        case OP_LOADL:
            const char* vname;

            // decodes reference name in local symbol table
            for (size_t i0 = 0; i0 < p->symbol_table.size; i0++) {
                if (((Value*)p->symbol_table.values[i0])->value.to_int == i.sx.sx) {
                    vname = p->symbol_table.keys[i0];
                    break;
                }
            }
            
            sprintf(buf, "%s %i (%s)", operation_strings[i.sx.op], i.sx.sx, vname);
            break;

        default:
            failure("Failed to disassemble instruction!");
    }
    return buf;
}

void compilererr(program* p, lxpos pos, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d):\n", ERR_COL, msg, pos.line_pos + 1, pos.col_pos + 1);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", pos.line_pos + 1, get_line(p->src_code, pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + pos.col_pos), DEF_COL);
    exit(0);
}