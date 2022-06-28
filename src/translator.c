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
            translate_expression(p, vector_get(&n->children, 0));
            short g_addr = register_global(p, n);
            p->instructions[p->size++] = encode_instruction_R_i(OP_STG, --p->translator.sp, g_addr);
            break;
        
        default:
            translate_err(p, n, "Invalid statement node");
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
            p->instructions[p->size++] = encode_instruction_3R(OP_NEG, p->translator.sp, p->translator.sp, 0);
            break;

        case AST_INTEGER:
            short k_address = register_constant(p, n); 
            p->instructions[p->size++] = encode_instruction_R_i(OP_LDK, p->translator.sp++, k_address);
            break;

        case AST_REFERENCE:
            unsigned short g_addr = load_global(p, n);
            p->instructions[p->size++] = encode_instruction_R_i(OP_LDG, p->translator.sp++, g_addr);
            break;

        case AST_CALL:
            break;

        default:
            translate_err(p, n, "Invalid expression node");
            break;
    }
}

// --------------- SYMBOL/VAR STORAGE ----------------

uint16_t register_constant(program* p, astnode* k)
{
    TValue* k_addr = map_get(&p->translator.constant_table, k->value);

    if (k_addr == NULL) 
    {
        TValue* index = TInt(p->translator.constant_table.size);
        map_put(&p->translator.constant_table, k->value, index);
        p->constants[index->value.i] = *coerce_constant(p, k);
        return index->value.i;
    }
    else
    {
        return k_addr->value.i;
    }
}

uint16_t register_global(program* p, astnode* g)
{
    TValue* g_addr = map_get(&p->translator.symbol_table, g->value);

    if (g_addr == NULL) 
    {
        TValue* index = TInt(p->translator.symbol_table.size);
        map_put(&p->translator.symbol_table, g->value, index);
        return index->value.i;
    }
    else
    {
        return g_addr->value.i;
    }
}

uint16_t load_global(program* p, astnode* g)
{
    TValue* g_addr = map_get(&p->translator.symbol_table, g->value);

    if (g_addr == NULL) {
        translate_err(p, g, "Undeclared variable cannot be dereferenced");
    }

    return g_addr->value.i;
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
    "OP_MOV",
    "OP_LDK",
    "OP_LDG",
    "OP_STG",
    "OP_ADD",
    "OP_SUB",
    "OP_MUL",
    "OP_DIV",
    "OP_NEG",     // 8
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
            TValue* index = TInt((ins >> 16) & 0xffff);
            sprintf(buf, "%s %i %i \t (%s)", ASM_OP_STR[op], (ins >> 8) & 0xff, index->value.i, map_get_key(&p->translator.symbol_table, index));
            break;

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