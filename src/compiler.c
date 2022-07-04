#include "exec.h"

vm_op decode_op(const char* operator);

void compile(program* p, astnode* root)
{
    for (size_t i = 0; i < root->children.size; i++)
    {
        compile_statement(p, vector_get(&root->children, i));
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
        
        default:
            compilererr(p, statement->pos, "Failed to compile statement into bytecode!");
    }
}

void compile_assignment(program* p, astnode* s)
{
    vm_scope scope;

    compile_expression(p, vector_get(&s->children, 0));
    p->code[p->length].ux.op = OP_STORG;
    p->code[p->length].ux.ux = register_variable(p, s->value, &scope);
    p->length++;
}

void compile_call(program* p, astnode* call)
{

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

size_t register_constant(program* p, Value v)
{
    Value* address;

    if ((address = map_get(&p->constant_table, value_to_str(&v))) == NULL) 
    {
        address = malloc(sizeof(Value));
        address->type = VM_INT;
        address->value.to_int = p->constant_table.size;
        
        map_put(&p->constant_table, value_to_str(&v), address);
        p->constants[address->value.to_int] = v;
    }
    
    return address->value.to_int;
}

size_t register_variable(program* p, const char* name, vm_scope* scope)
{
    size_t address = dereference_variable(p, name, scope);

    if (address == (-1UL)) {
        Value* a = malloc(sizeof(Value));
        
    } else {
        return address;
    }
}

size_t dereference_variable(program* p, const char* name, vm_scope* scope)
{
    Value* address = map_get(&p->symbol_table, name);
    program* p0 = p;

    // iteratively determines variable location
    while (address == NULL && p0->prev != NULL) {
        p0 = p0->prev;
        address = map_get(&p0->symbol_table, name);
    }

    // determines scope of variable
    if (p0->prev == NULL) {
        *scope = VM_GLOBAL_SCOPE;
    } else if (p0 == p) {
        *scope = VM_LOCAL_SCOPE;
    } else {
        *scope = VM_CLOSED_SCOPE;
    }
    
    return address == NULL ? -1 : address->value.to_int;
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
    "ADD     ",
    "SUB     ",
    "MUL     ",
    "DIV     ",
    "NEG     ",
    "PUSHK   ",
    "STORG   ",
    "LOADG   ",
};

const char* disassemble(instruction i) {
    char* buf = malloc(sizeof(char) * 64);

    switch (i.stackop.op)
    {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_NEG:
            sprintf(buf, "%s", operation_strings[i.stackop.op]);
            break;
        
        case OP_PUSHK:
        case OP_STORG:
        case OP_LOADG:
            sprintf(buf, "%s %u", operation_strings[i.stackop.op], i.ux.ux);
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