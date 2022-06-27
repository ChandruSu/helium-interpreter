#include "he.h"


int main(int argc, const char* argv[])
{
    const char* src;

    if (argc == 3)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            src = read_file(argv[2]);
        } 
        else if (strcmp(argv[1], "-s") == 0)
        {
            src = argv[2];
        }
    } 
    else 
    {
        fprintf(stderr, "%s Invalid number of arguments recieved!", ERROR);
        exit(0);
    }

    // lexical analysis
    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);

    lexer lx = lexer_new(src);
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);
    
    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken_display(tokens.items[i]);
    }

    // abstract syntax parsing
    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p);
    printf("%s\n\n", astnode_tostr(tree));

    for (size_t i = 0; i < tokens.size; i++)
    {
        free(tokens.items[i]);
    }

    // interpreting
    interpreter in = {
        .program = {
            .instructions = (unsigned int*) malloc(sizeof(unsigned int) * 1000),
            .size = 0,
            .constants = (TValue*) malloc(sizeof(TValue) * MAX_CONSTANTS),
            
            .source = src
        },
        .vm = {
            .calls     = (call_info*) malloc(sizeof(call_info) * MAX_CALLS),
            .ci        = 0,
            .registers = (TValue*) malloc(sizeof(TValue) * MAX_REGISTERS),
            .heap      = (TValue*) malloc(sizeof(TValue) * MAX_HEAP_SIZE),
        },
    };

    printf("%s Beginning interpretting:\n\n", MESSAGE);

    translate_ast(&in.program, tree);


    for (size_t i = 0; i < 5; i++)
    {
        TValue* k = &in.program.constants[i];
        printf("Constant %li = %s\n", i, TValue_tostr(k));
    }
    

    printf("\nInstructions: \n");
    for (size_t i = 0; i < in.program.size; i++)
    {
        int ins = in.program.instructions[i];
        printf("%i %i %i %i\n", ins & 0xff, (ins >> 8) & 0xff, (ins >> 16) & 0xff, (ins >> 24) & 0xff);
    }

    return 0;
}