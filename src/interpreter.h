#ifndef HE_INTERPRETER_HEADER
#define HE_INTERPRETER_HEADER

#include "parser.h"

typedef struct interpreter {
    astnode* root;
} interpreter;

void interpret(interpreter* in);

void interpret_expression(interpreter* in);

#endif