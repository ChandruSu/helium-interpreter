#include "interpreter.h"

void interpret(interpreter* in, astnode* n)
{
    interpret_expression(in, n);
}

void interpret_expression(interpreter* in, astnode* n)
{
    switch (n->type)
    {
        case AST_BINARY_EXPRESSION:
            break;
        
        case AST_UNARY_EXPRESSION:
            break;

        case AST_INTEGER:
            break;

        case AST_REFERENCE:
            break;

        case AST_CALL:
            break;

        default:
            break;
    }
}

// Intermediate low-level bytecode instruction:
//      OP (8bits) R0 (8bits) R1 (8bits) R2 (8bits)
//
unsigned int encode_instruction_3R(vm_op op, short r0, short r1, short r2)
{
    return op | ((r0 & 0xff) << 8) | ((r1 & 0xff) << 16) | ((r2 & 0xff) << 24);
}