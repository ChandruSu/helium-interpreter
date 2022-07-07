#include "he.h"


int main(int argc, const char* argv[])
{
    const char* src;

    if (argc < 3) {
        fprintf(stderr, "%s Invalid number of arguments recieved!", ERROR);
        exit(0);
    } else {
        src = read_file(argv[2]);
    }

    printf("\n%s Reading code:\n\n%s\n", MESSAGE, src);

    // lexical analysis
    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);

    lexer lx = lexer_new(src);
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);
    
    for (size_t i = 0; i < tokens.size; i++)
        lxtoken_display(tokens.items[i]);

    // syntax parsing
    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p);
    printf("%s\n", astnode_tostr(tree));

    // byte code compilation
    printf("\n%s Beginning compilation:\n\n", MESSAGE);

    program pp = {
        .code = malloc(sizeof(instruction) * 0xff),
        .length = 0,
        .argc = 0,
        .constants = malloc(sizeof(Value) * 0xff),
        .src_code = src,
        .prev = NULL,

        .constant_table = map_new(37),
        .symbol_table = map_new(37)
    };
    
    compile(&pp, tree);
    printf(disassemble_program(&pp));
    
    // execution
    printf("\n%s Beginning bytecode execution:\n\n", MESSAGE);
    
    virtual_machine vm = {
        .ci = -1,
        .call_stack = malloc(sizeof(call_info) * 32),
        .heap = malloc(sizeof(Value) * 0xff),
        .stack = malloc(sizeof(Value) * 0xff),
    };

    run_program(&vm, &pp);

    printf("\n");
    for (size_t i = 0; i < 0xf; i++)
    {
        printf("Stack %li = %s\n", i, value_to_str(&vm.stack[i]));
    }

    printf("\n");
    for (size_t i = 0; i < 0xf; i++)
    {
        printf("Heap %li = %s\n", i, value_to_str(&vm.heap[i]));
    }

    printf("\n%s Program has ended successfully!\n\n", MESSAGE);
    return 0;
}