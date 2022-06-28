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

    printf("%s Reading code:\n\n%s\n", MESSAGE, src);

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
            .instructions = (uint32_t*) malloc(sizeof(uint32_t) * 1000),
            .size         = 0,
            .constants    = (TValue*) malloc(sizeof(TValue) * MAX_CONSTANTS),
            .source       = src,
            .translator = {
                .sp = 0,
                .constant_table = map_new(21),
                .symbol_table = map_new(21),
            }
        },
        .vm = {
            .top       = 0,
            .ccall     = NULL,
            .calls     = (call_info*) malloc(sizeof(call_info) * MAX_CALLS),
            .registers = (TValue*) malloc(sizeof(TValue) * MAX_REGISTERS),
            .heap      = (TValue*) malloc(sizeof(TValue) * MAX_HEAP_SIZE),
        },
    };

    printf("%s Beginning translation:\n\n", MESSAGE);

    translate_ast(&in.program, tree);
    
    printf("Instructions: \n");
    for (size_t i = 0; i < in.program.size; i++)
    {
        int ins = in.program.instructions[i];
        printf("%s\n", disassemble_instruction(&in.program, ins));
    }

    printf("%s Beginning execution:\n\n", MESSAGE);
    execute_program(&in);
    
    printf("\nOutput:\n");
    for (size_t i = 0; i < in.program.translator.symbol_table.size; i++)
    {
        printf("Global %li = %s\n", i, TValue_tostr(&in.vm.heap[i]));
    }
    
    printf("%sProgram has ended successfully!\n", MESSAGE);

    return 0;
}