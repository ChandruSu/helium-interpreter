#include "interpreter.h"

TValue* coerce_constant(program* p, astnode* k_node);

// -------------- TRANSLATION METHODS ----------------

void translate_ast(program* p, astnode* n)
{
    for (size_t i = 0; i < n->children.size; i++)
    {
        translate_statement(p, vector_get(&n->children, i));
    }
}

void translate_statement(program* p, astnode* n)
{
    switch (n->type)
    {
        case AST_ASSIGN:
            astnode* rhs = vector_get(&n->children, 0);
            
            if (rhs->type == AST_FUNCTION) {
                uint16_t code_addr = translate_function_definition(p, rhs);
                p->instructions[p->size++] = encode_instruction_R_i(OP_LDK, p->translator.sp++, code_addr);
            } else {
                translate_expression(p, rhs);
            }

            vm_scope scope;
            vm_op operation;
            uint16_t g_addr = register_variable(p, n->value, &scope);

            switch (scope) {
                case SCOPE_LOCAL: operation = OP_STL; break;
                case SCOPE_GLOBAL: operation = OP_STG; break;
                default: translate_err(p, n, "Invalid scope operation!");
            }

            p->instructions[p->size++] = encode_instruction_R_i(operation, --p->translator.sp, g_addr);
            break;
        
        case AST_CALL:
        default:
            translate_err(p, n, "Invalid statement node - could not translate");
            break;
    }
}

void translate_expression(program* p, astnode* n)
{
    switch (n->type)
    {
        case AST_BINARY_EXPRESSION:
            translate_expression(p, vector_get(&n->children, 0));
            translate_expression(p, vector_get(&n->children, 1));

            vm_op op_code;
            if (streq(n->value, "+")) {
                op_code = OP_ADD;
            } else if (streq(n->value, "-")) {
                op_code = OP_SUB;
            } else if (streq(n->value, "*")) {
                op_code = OP_MUL;
            } else if (streq(n->value, "/")) {
                op_code = OP_DIV;
            } 

            p->translator.sp -= 1;
            p->instructions[p->size++] = encode_instruction_3R(op_code, p->translator.sp - 1, p->translator.sp - 1, p->translator.sp);
            break;

        case AST_UNARY_EXPRESSION:
            translate_expression(p, vector_get(&n->children, 0));
            p->instructions[p->size++] = encode_instruction_3R(OP_NEG, p->translator.sp - 1, p->translator.sp - 1, 0);
            break;

        case AST_INTEGER:
            short k_address = register_constant(p, n->value, *coerce_constant(p, n)); 
            p->instructions[p->size++] = encode_instruction_R_i(OP_LDK, p->translator.sp++, k_address);
            break;

        case AST_REFERENCE:
            vm_scope scope;
            vm_op operation;
            uint16_t g_addr = dereference_variable(p, n->value, &scope);
            
            switch (scope) {
                case SCOPE_LOCAL: operation = OP_LDL; break;
                case SCOPE_GLOBAL: operation = OP_LDG; break;
                case SCOPE_CLOSED: operation = OP_LDC; break;
                default: translate_err(p, n, "Invalid scope operation!");
            }

            p->instructions[p->size++] = encode_instruction_R_i(operation, p->translator.sp++, g_addr);
            break;

        case AST_CALL:
        default:
            translate_err(p, n, "Invalid expression node - could not translate");
            break;
    }
}

uint16_t translate_function_definition(program* p, astnode* f)
{
    program* p0 = malloc(sizeof(program));
    
    p0->instructions = (uint32_t*) malloc(sizeof(uint32_t) * 1000);
    p0->size = 0;
    p0->constants = malloc(sizeof(TValue) * 0xff);
    p0->source = p->source;
    p0->translator.sp = 0;
    p0->translator.constant_table = map_new(23);
    p0->translator.symbol_table = map_new(23);
    p0->prev = p;

    astnode* params = vector_get(&f->children, 0);
    for (size_t i = 0; i < params->children.size; i++) {
        astnode* symbol = vector_get(&params->children, i);
        map_put(&p->translator.symbol_table, symbol->value, TInt(-i - 1));
    }

    astnode* code = vector_get(&f->children, 1);
    for (size_t i = 0; i < code->children.size; i++) {
        translate_statement(p0, vector_get(&code->children, i));
    }
    
    TValue* code_object = malloc(sizeof(TValue));
    code_object->type = TYPE_POINTER;
    code_object->value.pp = p0;

    char* buf = malloc(sizeof(char) * 16);
    sprintf(buf, "<code %p>", p0);

    disassemble_program(p0);

    return register_constant(p, buf, *code_object);
}

// --------------- SYMBOL/VAR STORAGE ----------------

uint16_t register_constant(program* p, const char* cname, TValue val)
{
    TValue* k_addr = map_get(&p->translator.constant_table, cname);

    if (k_addr == NULL) 
    {
        TValue* index = TInt(p->translator.constant_table.size);
        map_put(&p->translator.constant_table, cname, index);
        p->constants[index->value.i] = val;
        return index->value.i;
    }
    else
    {
        return k_addr->value.i;
    }
}

uint16_t register_variable(program* p, const char* vname, vm_scope* scope)
{
    uint16_t addr = dereference_variable(p, vname, scope);

    // registers new variables
    if (addr == (uint16_t)(-1)) 
    {
        TValue* index = TInt(p->translator.symbol_table.size);
        map_put(&p->translator.symbol_table, vname, index);

        if (p->prev == NULL)
            *scope = SCOPE_GLOBAL;
        else
            *scope = SCOPE_LOCAL;
        
        return index->value.i;
    } 
    else 
    {
        return addr;
    }
}

uint16_t dereference_variable(program* p, const char* vname, vm_scope* scope)
{
    program* p0 = p;
    TValue* addr = map_get(&p->translator.symbol_table, vname);

    // iterative search for variable address
    while (addr == NULL && p0->prev != NULL) {
        p0 = p0->prev;
        addr = map_get(&p0->translator.symbol_table, vname);
    }
    
    if (addr == NULL) 
    {
        *scope = SCOPE_NONE;
        return -1;
    }
    
    // returns variable scope for registered variable
    if (p0->prev == NULL) {
        *scope = SCOPE_GLOBAL;
    } else if (p0 == p) {
        *scope = SCOPE_LOCAL;
    } else {
        *scope = SCOPE_CLOSED;
    }

    return addr->value.i;
}

// -------------------- ENCODING ---------------------

uint32_t encode_instruction_3R(vm_op op, uint8_t r0, uint8_t r1, uint8_t r2)
{
    return (op & 0xff) | ((r0 & 0xff) << 8) | ((r1 & 0xff) << 16) | ((r2 & 0xff) << 24);
}

uint32_t encode_instruction_R_i(vm_op op, uint8_t r0, uint16_t i0)
{
    return (op & 0xff) | ((r0 & 0xff) << 8) | ((i0 & 0xffff) << 16);
}

// ---------------------- UTILS ----------------------

const char* ASM_OP_STR[] = {
    "OP_MOV",     // 0
    "OP_LDK",
    "OP_LDG",
    "OP_STG",
    "OP_LDL",     // 4
    "OP_STL",
    "OP_LDC",
    "OP_ADD",
    "OP_SUB",     // 8
    "OP_MUL",
    "OP_DIV",
    "OP_NEG",
};

TValue* coerce_constant(program* p, astnode* k_node)
{
    switch (k_node->type)
    {
        case TYPE_INT:
            return TInt(atoi(k_node->value));
        
        case TYPE_FLOAT:
            return TFloat(atof(k_node->value));
        
        case TYPE_STRING:
            return TString(k_node->value);

        default:
            translate_err(p, k_node, "Cannot coerce token into a value");
    }
    return NULL;
}

void disassemble_program(program* p)
{
    for (size_t i = 0; i < p->size; i++)
    {
        int ins = p->instructions[i];
        printf("%s\n", disassemble_instruction(p, ins));
    }

    printf("\n");
}

const char* disassemble_instruction(program* p, uint32_t ins)
{
    vm_op op = (ins & 0xff);
    char* buf = (char*)malloc(sizeof(char) * 100);

    switch (op)
    {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOV:
        case OP_NEG:
            sprintf(buf, "%s %i %i %i", ASM_OP_STR[op], (ins >> 8) & 0xff, (ins >> 16) & 0xff, (ins >> 24) & 0xff);
            break;
        
        case OP_LDK:
            uint16_t address = (ins >> 16) & 0xffff;
            sprintf(buf, "%s %i %i \t (%s)", ASM_OP_STR[op], (ins >> 8) & 0xff, address, TValue_tostr(&p->constants[address]));
            break;

        case OP_LDG:
        case OP_STG:
            program* p0 = p;
            while (p0->prev != NULL) p0 = p->prev;

            TValue* index = TInt((ins >> 16) & 0xffff);
            sprintf(buf, "%s %i %i \t (%s)", ASM_OP_STR[op], (ins >> 8) & 0xff, index->value.i, map_get_key(&p0->translator.symbol_table, index));
            break;
        case OP_LDL:
        case OP_STL:
            index = TInt((ins >> 16) & 0xffff);
            sprintf(buf, "%s %i %i \t (%s)", ASM_OP_STR[op], (ins >> 8) & 0xff, index->value.i, map_get_key(&p->translator.symbol_table, index));
            break;

        case OP_LDC:
        default:
            break;
    }

    return buf;
}

void translate_err(program* p, astnode* node, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d):\n", ERR_COL, msg, node->pos.line_pos + 1, node->pos.col_pos + 1);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", node->pos.line_pos + 1, get_line(p->source, node->pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + node->pos.col_pos), DEF_COL);
    exit(0);
}