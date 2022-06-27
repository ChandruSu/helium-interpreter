#include "interpreter.h"

TValue* coerce_constant(program* p, astnode* k_node);

// -------------- TRANSLATION METHODS ----------------

void translate_ast(program* p, astnode* n)
{
    translator t = {
        .sp = 0,
        .constant_table = map_new(21),
        .symbol_table = map_new(21),
    };

    for (size_t i = 0; i < n->children.size; i++)
    {
        translate_statement(p, &t, vector_get(&n->children, i));
    }
}

void translate_statement(program* p, translator* t, astnode* n)
{
    switch (n->type)
    {
        case AST_ASSIGN:
            translate_expression(p, t, vector_get(&n->children, 0));
            short g_addr = register_global(p, t, n);
            p->instructions[p->size++] = encode_instruction_R_i(OP_STG, --t->sp, g_addr);
            break;
        
        default:
            translate_err(p, n, "Invalid statement node");
            break;
    }
}

void translate_expression(program* p, translator* t, astnode* n)
{
    switch (n->type)
    {
        case AST_BINARY_EXPRESSION:
            translate_expression(p, t, vector_get(&n->children, 0));
            translate_expression(p, t, vector_get(&n->children, 1));

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

            t->sp -= 1;
            p->instructions[p->size++] = encode_instruction_3R(op_code, t->sp - 1, t->sp - 1, t->sp);
            break;
        
        case AST_UNARY_EXPRESSION:
            translate_expression(p, t, vector_get(&n->children, 0));
            p->instructions[p->size++] = encode_instruction_3R(OP_NEG, t->sp, t->sp, 0);
            break;

        case AST_INTEGER:
            short k_address = register_constant(p, t, n); 
            p->instructions[p->size++] = encode_instruction_R_i(OP_LDK, t->sp++, k_address);
            break;

        case AST_REFERENCE:
            unsigned short g_addr = load_global(p, t, n);
            p->instructions[p->size++] = encode_instruction_R_i(OP_LDG, t->sp++, g_addr);
            break;

        case AST_CALL:
            break;

        default:
            translate_err(p, n, "Invalid expression node");
            break;
    }
}

// --------------- SYMBOL/VAR STORAGE ----------------

unsigned short register_constant(program* p, translator* t, astnode* k)
{
    TValue* k_addr = map_get(&t->constant_table, k->value);

    if (k_addr == NULL) 
    {
        TValue* index = TInt(t->constant_table.size);
        map_put(&t->constant_table, k->value, index);
        p->constants[index->value.i] = *coerce_constant(p, k);
        return index->value.i;
    }
    else
    {
        return k_addr->value.i;
    }
}

unsigned short register_global(program* p, translator* t, astnode* g)
{
    TValue* g_addr = map_get(&t->symbol_table, g->value);

    if (g_addr == NULL) 
    {
        TValue* index = TInt(t->symbol_table.size);
        map_put(&t->symbol_table, g->value, index);
        return index->value.i;
    }
    else
    {
        return g_addr->value.i;
    }
}

unsigned short load_global(program* p, translator* t, astnode* g)
{
    TValue* g_addr = map_get(&t->symbol_table, g->value);

    if (g_addr == NULL) {
        translate_err(p, g, "Undeclared variable cannot be dereferenced");
    }

    return g_addr->value.i;
}

// -------------------- ENCODING ---------------------

unsigned int encode_instruction_3R(vm_op op, short r0, short r1, short r2)
{
    return (op & 0xff) | ((r0 & 0xff) << 8) | ((r1 & 0xff) << 16) | ((r2 & 0xff) << 24);
}

unsigned int encode_instruction_R_i(vm_op op, short r0, short i)
{
    return (op & 0xff) | ((r0 & 0xff) << 8) | ((i & 0xffff) << 16);
}

// ---------------------- UTILS ----------------------

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

void translate_err(program* p, astnode* node, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d):\n", ERR_COL, msg, node->pos.line_pos + 1, node->pos.col_pos + 1);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", node->pos.line_pos + 1, get_line(p->source, node->pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + node->pos.col_pos), DEF_COL);
    exit(0);
}