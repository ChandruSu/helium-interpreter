#include "exec.h"

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
            break;
        
        default:
            compilererr(p, statement->pos, "Failed to compile statement into bytecode!");
    }
}

void compile_assignment(program* p, astnode* s)
{
    
}

void compile_expression(program* p, astnode* expression)
{

}

void compilererr(program* p, lxpos pos, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d):\n", ERR_COL, msg, pos.line_pos + 1, pos.col_pos + 1);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", pos.line_pos + 1, get_line(p->src_code, pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + pos.col_pos), DEF_COL);
    exit(0);
}
