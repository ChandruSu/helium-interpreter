// #define HE_DEBUG_MODE

#include "he.h"
#include <time.h>

virtual_machine* current_vm;

int main(int argc, const char* argv[])
{
    const char* src;
    char fpath[256];

    if (argc < 2) {
        failure("File not specified!");
    } else {
        sprintf(fpath, "%s/%s", getcwd(fpath, sizeof(fpath)), argv[1]);
        src = read_file(fpath);
    }

#ifdef HE_DEBUG_MODE
    printf("\n%s Reading code:\n\n%s\n", MESSAGE, src);

    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);
#endif

    vector tokens = vector_new(64);
    lexer lx = lexer_new(src, fpath);
    lexify(&lx, &tokens);
    
#ifdef HE_DEBUG_MODE
    for (size_t i = 0; i < tokens.size; i++) {
        lxtoken_display(tokens.items[i]);
    }

    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);
#endif

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p);

#ifdef HE_DEBUG_MODE
    printf("%s\n", astnode_tostr(tree));

    printf("\n%s Beginning compilation:\n\n", MESSAGE);
#endif

    program pp = {
        .code = malloc(sizeof(instruction) * MAX_LOCAL_VARIABLES),
        .length = 0,
        .argc = 0,
        .constants = malloc(sizeof(Value) * MAX_LOCAL_CONSTANTS),
        .prev = NULL,

        .constant_table = map_new(37),
        .symbol_table = map_new(37),
        .closure_table = map_new(37),
        .line_address_table = map_new(37),
    };
    
    register_all_natives(&pp);
    compile(&pp, tree);

#ifdef HE_DEBUG_MODE
    printf(disassemble_program(&pp));
    
    printf("\n%s Beginning bytecode execution:\n\n", MESSAGE);
    
    clock_t begin = clock();
#endif

    virtual_machine vm = {
        .ci = -1,
        .call_stack = malloc(sizeof(call_info) * MAX_CALL_STACK),
        .heap = calloc(MAX_HEAP_SIZE, sizeof(Value)),
        .stack = calloc(MAX_STACK_SIZE, sizeof(Value)),
    };

    current_vm = &vm;

    run_program(&vm, NULL, vCode(&pp, NULL).value.to_code);

#ifdef HE_DEBUG_MODE
    clock_t end = clock();
    double time_spent = 1000 * (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n%s Execution took %f milliseconds!\n\n", MESSAGE, time_spent);
    
    for (size_t i = 0; i < 0x20; i++)
    {
        printf("Stack %li = %s\n", i, value_to_str(&vm.stack[i]));
    }

    printf("\n");
    for (size_t i = 0; i < 0x20; i++)
    {
        printf("Heap %li = %s\n", i, value_to_str(&vm.heap[i]));
    }
    
    printf("\n%s Program has ended successfully!\n\n", MESSAGE);
#endif
    return 0;
}